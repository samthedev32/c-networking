#include <libwebsockets.h>
#include <pthread.h>
#include <string.h>

#define PORT 443
#define ADDRESS "localhost"

static int callback(struct lws *wsi, enum lws_callback_reasons reason,
                    void *user, void *in, size_t len);

void *service(void *arg);

int main()
{
  // Init lws
  static struct lws_protocols protocols[] = {{"ws", callback}, {}};

  struct lws_context_creation_info info = {};

  info.port = CONTEXT_PORT_NO_LISTEN; /* we do not run any server */
  info.protocols = protocols;

  struct lws_context *context = lws_create_context(&info);

  if (!context)
  {
    printf("Failed to create context\n");
    return 1;
  }

  // Connect
  struct lws_client_connect_info i = {};

  i.protocol = i.local_protocol_name = "ws";
  i.address = i.host = i.origin = ADDRESS;
  i.context = context;
  i.port = PORT;

  struct lws *wsi = lws_client_connect_via_info(&i);

  if (!wsi)
  {
    printf("Failed to connect\n");
    return 2;
  }

  // Start Service Thread
  pthread_t thread;
  pthread_create(&thread, NULL, service, context);

  // Send Messages
  // lws_callback_on_writable(wsi);
  while (1)
  {
    char buf[LWS_PRE + 256];
    scanf("%[^\n]s", buf + LWS_PRE);
    size_t len = strlen(buf + LWS_PRE);

    lws_write(wsi, (unsigned char *)&buf[LWS_PRE], len, LWS_WRITE_TEXT);
    // TODO: fix
  }

  // Cleanup
  pthread_cancel(thread);
  pthread_join(thread, NULL);
  lws_context_destroy(context);
  return 0;
}

void *service(void *arg)
{
  struct lws_context *context = arg;

  pthread_detach(pthread_self());

  while (1)
  {
    lws_service(context, 0);
  }

  pthread_exit(NULL);
  return NULL;
}

int callback(struct lws *wsi, enum lws_callback_reasons reason,
             void *user, void *in, size_t len)
{
  switch (reason)
  {
  case LWS_CALLBACK_CLIENT_ESTABLISHED:
  {
    printf("connection established\n");
  }
  break;

  case LWS_CALLBACK_CLIENT_WRITEABLE:
  {
  }
  break;

  case LWS_CALLBACK_CLIENT_CLOSED:
  {
    printf("connection closed\n");
  }

  default:
    break;
  }

  return 0;
}