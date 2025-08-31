# log
echo "[docker] Starting identity node on $(hostname -I | awk '{print $1}')"

./build/identity_server & sleep 1.0;
cd example && go run main.go; cd -

# log
echo "[docker] identity node up"