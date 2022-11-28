#include "changelly_plugin.h"

void handle_provide_token(void *parameters) {
    ethPluginProvideInfo_t *msg = (ethPluginProvideInfo_t *) parameters;
    context_t *context = (context_t *) msg->pluginContext;
    PRINTF("Changelly plugin provide token: 0x%p, 0x%p\n", msg->item1, msg->item2);

    if (ADDRESS_IS_NETWORK_TOKEN(context->input_token)) {
        context->input_decimals = WEI_TO_ETHER;
        context->token_found |= TOKEN_SENT_FOUND;
    } else if (msg->item1 != NULL) {
        context->input_decimals = msg->item1->token.decimals;
        strlcpy(context->input_ticker,
                (char *) msg->item1->token.ticker,
                sizeof(context->input_ticker));
        context->token_found |= TOKEN_SENT_FOUND;
    } else {
        // Did not find the token and token is not ETH.
        context->input_decimals = DEFAULT_DECIMAL;
        // We will need an additional screen to display a warning message.
        msg->additionalScreens++;
    }

    if (ADDRESS_IS_NETWORK_TOKEN(context->output_token)) {
        context->output_decimals = WEI_TO_ETHER;
        context->token_found |= TOKEN_RECEIVED_FOUND;
    } else if (msg->item2 != NULL) {
        context->output_decimals = msg->item2->token.decimals;
        strlcpy(context->output_ticker,
                (char *) msg->item2->token.ticker,
                sizeof(context->output_ticker));
        context->token_found |= TOKEN_RECEIVED_FOUND;
    } else {
        // Did not find the token and token is not ETH.
        context->output_decimals = DEFAULT_DECIMAL;
        // We will need an additional screen to display a warning message.
        msg->additionalScreens++;
    }

    msg->result = ETH_PLUGIN_RESULT_OK;
}