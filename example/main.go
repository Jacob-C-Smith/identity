package main

import (
	"crypto/sha256"
	"encoding/base64"
	"example/identity"
	"fmt"
	"log"
	"net/http"
	"strings"
	"time"
)

// Minimal HTTP server demonstrating RFC 7235 authentication framework.
// - Sends 401 with one or more WWW-Authenticate challenges.
// - Validates Basic credentials and grants access when correct.
var id *identity.Identity = nil

func main() {
	var addr string = ":6712"
	var realm string = "g10.app"
	var user string = "jacob"
	var pass string = "secret"

	id = identity.NewIdentity("localhost:6708", 5*time.Second)

	mux := http.NewServeMux()
	mux.Handle("/", protectedHandler(realm, user, pass))

	s := &http.Server{
		Addr:              addr,
		Handler:           mux,
		ReadHeaderTimeout: 5 * time.Second,
	}

	if err := s.ListenAndServe(); err != nil && err != http.ErrServerClosed {
		log.Fatal(err)
	}
}

func protectedHandler(realm, wantUser, wantPass string) http.Handler {
	return http.HandlerFunc(func(w http.ResponseWriter, r *http.Request) {
		// Check Authorization header
		authz := r.Header.Get("Authorization")
		if checkBasic(authz, wantUser, wantPass) == "\"okay\"" {
			w.Header().Add("WWW-Authenticate", fmt.Sprintf(`Basic realm="%s", charset="UTF-8"`, realm))
			w.Header().Add("WWW-Authenticate", `Newauth realm="apps", type=1, title="Login to apps"`)
			http.SetCookie(w, &http.Cookie{
				Name:     "g10_auth",
				Value:    "okay",
				Path:     "/",
				Domain:   ".g10.app",
				HttpOnly: true,
				Secure:   true,
				SameSite: http.SameSiteLaxMode,
				Expires:  time.Now().Add(24 * time.Hour),
			})
			w.Header().Set("Content-Type", "text/plain; charset=utf-8")
			w.WriteHeader(http.StatusUnauthorized)
			fmt.Fprintln(w, "Unauthorized")
			return
		}

		// Authenticated
		w.Header().Set("Content-Type", "text/plain; charset=utf-8")
		fmt.Fprintf(w, "Authenticated\n")
	})
}

func checkBasic(header, wantUser, wantPass string) string {
	if header == "" {
		return "not okay"
	}
	scheme, b64, ok := strings.Cut(header, " ")
	if !ok || !strings.EqualFold(scheme, "Basic") {
		return "not okay"
	}
	dec, err := base64.StdEncoding.DecodeString(b64)
	if err != nil {
		return "not okay"
	}
	creds := string(dec)
	user, pass, ok := strings.Cut(creds, ":")
	if !ok {
		return "not okay"
	}
	sum := sha256.Sum256([]byte(pass))
	pass = fmt.Sprintf("%x", sum[:])
	fmt.Printf("user=%s,pass=%s\n", user, pass)
	s, _ := id.Authenticate(user, pass)

	return s
}
