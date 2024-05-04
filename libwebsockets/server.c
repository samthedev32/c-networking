#include <libwebsockets.h>

#define PORT 443

static int callback(struct lws *wsi, enum lws_callback_reasons reason,
                    void *user, void *in, size_t len);

int main()
{
    // Start Server
    struct lws_protocols protocols[] = {{"ws", callback}, {}};
    struct lws_context_creation_info info = {};

    info.port = PORT;
    info.protocols = protocols;

    struct lws_context *context = lws_create_context(&info);

    if (!context)
    {
        printf("Failed to start Server\n");
        return 1;
    }

    // Run Server
    while (1)
    {
        lws_service(context, 0);
    }

    // Return
    lws_context_destroy(context);
    return 0;
}

static int callback(struct lws *wsi, enum lws_callback_reasons reason,
                    void *user, void *in, size_t len)
{
    switch (reason)
    {
    case LWS_CALLBACK_SERVER_NEW_CLIENT_INSTANTIATED:
        printf("new connection\n");
        break;

    case LWS_CALLBACK_RECEIVE:
        printf("received data: %.*s\n", len, (char *)in);
        break;

    default:
        break;
    }

    return 0;
}