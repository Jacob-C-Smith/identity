package identity

import (
	"bufio"
	"encoding/json"
	"fmt"
	"net"
	"strings"
	"sync"
	"time"
)

type Identity struct {
	addr    string
	timeout time.Duration

	mu   sync.Mutex
	conn net.Conn
	rd   *bufio.Reader
	wr   *bufio.Writer
}

type AuthenticationRequest struct {
	Username string `json:"user"`
	Password string `json:"pass"`
}

func NewIdentity(addr string, timeout time.Duration) *Identity {
	id := &Identity{
		addr:    addr,
		timeout: timeout,
	}
	conn, err := net.DialTimeout("tcp", addr, timeout)
	if err == nil {
		id.conn = conn
		id.rd = bufio.NewReader(conn)
		id.wr = bufio.NewWriter(conn)
	}
	return id
}

func (id *Identity) Close() error {
	id.mu.Lock()
	defer id.mu.Unlock()
	if id.conn != nil {
		err := id.conn.Close()
		id.conn = nil
		id.rd = nil
		id.wr = nil
		return err
	}
	return nil
}

func (id *Identity) Authenticate(user, pass string) (string, error) {
	id.mu.Lock()
	defer id.mu.Unlock()

	req, err := json.Marshal(AuthenticationRequest{Username: user, Password: pass})
	if err != nil {
		return "", err
	}

	// write 8-byte big-endian length prefix followed by the payload
	l := uint64(len(req))
	hdr := []byte{
		byte(l),
		byte(l >> 8),
		byte(l >> 16),
		byte(l >> 24),
		byte(l >> 32),
		byte(l >> 40),
		byte(l >> 48),
		byte(l >> 56),
	}

	if id.wr != nil {
		if _, err = id.wr.Write(hdr); err != nil {
			return "", err
		}
		if _, err = id.wr.Write(req); err != nil {
			return "", err
		}
		if err = id.wr.Flush(); err != nil {
			return "", err
		}
	} else {
		writeAll := func(p []byte) error {
			for len(p) > 0 {
				n, err := id.conn.Write(p)
				if err != nil {
					return err
				}
				p = p[n:]
			}
			return nil
		}
		if err := writeAll(hdr); err != nil {
			return "", err
		}
		if err := writeAll(req); err != nil {
			return "", err
		}
	}
	fmt.Printf("wrote message\n")

	var res []byte = make([]byte, 8)
	id.conn.Read(res)

	var len uint64
	len = uint64(res[0]) |
		(uint64(res[1]) << 8) |
		(uint64(res[2]) << 16) |
		(uint64(res[3]) << 24) |
		(uint64(res[4]) << 32) |
		(uint64(res[5]) << 40) |
		(uint64(res[6]) << 48) |
		(uint64(res[7]) << 56)

	res = make([]byte, len)
	fmt.Printf("got %d bytes\n", len)

	id.conn.Read(res)

	var ress string = string(res)
	ress = strings.TrimRight(ress, "\x00")
	fmt.Printf("%+v\n", ress)

	return ress, nil
}
