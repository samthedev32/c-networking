# libwebsockets example

This is a simple websocket server-client example

```bash
echo "Building client"
gcc client.c -lwebsockets -o client

echo "Building server"
gcc server.c -lwebsockets -o server
```

> [!CAUTION]
> This example is not yet fully functional