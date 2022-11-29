#include "changelly_plugin.h"

void handle_finalize(void *parameters) {
    ethPluginFinalize_t *msg = (ethPluginFinalize_t *) parameters;
    context_t *context = (context_t *) msg->pluginContext;

    msg->numScreens = 2;

    if (!ADDRESS_IS_NETWORK_TOKEN(context->input_token)) {
        // Address is not network token (0xeee...) so we will need to look up the
        // token in the CAL
        msg->tokenLookup1 = context->input_token;
        PRINTF("Setting address sent to: %.*H\n", ADDRESS_LENGTH, context->input_token);

        // The user is not swapping ETH, so make sure there's no ETH being sent in
        // this tx.
        if (!allzeroes(msg->pluginSharedRO->txContent->value.value,
                       msg->pluginSharedRO->txContent->value.length)) {
            PRINTF("ETH attached to tx when token being swapped is %.*H\n",
                   sizeof(context->input_token),
                   context->input_token);
            msg->result = ETH_PLUGIN_RESULT_ERROR;
        }
    } else {
        msg->tokenLookup1 = NULL;
    }
    if (!ADDRESS_IS_NETWORK_TOKEN(context->output_token)) {
        // Address is not network token (0xeee...) so we will need to look up the
        // token in the CAL.
        PRINTF("Setting address receiving to: %.*H\n", ADDRESS_LENGTH, context->output_token);
        msg->tokenLookup2 = context->output_token;
    } else {
        msg->tokenLookup2 = NULL;
    }

    msg->uiType = ETH_UI_TYPE_GENERIC;
    msg->result = ETH_PLUGIN_RESULT_OK;
}
