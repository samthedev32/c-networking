#include <libwebsockets.h>

static int callback(struct lws *wsi, enum lws_callback_reasons reason,
                    void *user, void *in, size_t len) {
  switch (reason) {
  case LWS_CALLBACK_CLIENT_ESTABLISHED:
    printf("Connected to server\n");

    // Send a message to the server
    char buf[LWS_PRE + 128];

    const char msg[] = "Hello, server!";
    memcpy(&buf[LWS_PRE], msg, sizeof(msg));
    lws_write(wsi, (void *)&buf[LWS_PRE], sizeof(msg), LWS_WRITE_TEXT);
    break;

  case LWS_CALLBACK_CLIENT_RECEIVE:
    printf("Received data: %s\n", (char *)in);
    break;

  default:
    break;
  }

  return 0;
}

static struct lws_protocols protocols[] = {
    {"lws-minimal-client", callback, 0, 0}, {NULL, NULL, 0, 0}};

int main() {
  struct lws_context *context;

  /* Create Context */ {
    struct lws_context_creation_info info = {};

    info.options = LWS_SERVER_OPTION_DO_SSL_GLOBAL_INIT;
    info.port = CONTEXT_PORT_NO_LISTEN; /* we do not run any server */
    info.protocols = protocols;

    info.fd_limit_per_thread = 1 + 1 + 1;
    context = lws_create_context(&info);
    if (!context) {
      lwsl_err("lws init failed\n");
      return 1;
    }
  }

  struct lws_client_connect_info i = {};

  const char *address = "localhost";
  const int port = 12345;

  i.context = context;

  i.address = address;
  i.port = port;
  i.path = "/";

  i.host = i.address;
  i.origin = i.address;
  i.protocol = "ws";

  i.local_protocol_name = "lws-minimal-client";

  struct lws *wsi = lws_client_connect_via_info(&i);

  while (1) {
    lws_service(context, 1000);
    // Add your own logic here
  }

  lws_context_destroy(context);

  return 0;
}
