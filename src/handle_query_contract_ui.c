#include "changelly_plugin.h"

// Set UI for the "Send" screen.
static void set_send_ui(ethQueryContractUI_t *msg, const context_t *context) {
    strlcpy(msg->title, "Send", msg->titleLength);

    // set network ticker (ETH, BNB, etc) if needed
    if (ADDRESS_IS_NETWORK_TOKEN(context->input_token)) {
        const uint8_t *eth_amount = msg->pluginSharedRO->txContent->value.value;
        uint8_t eth_amount_size = msg->pluginSharedRO->txContent->value.length;

        amountToString(eth_amount,
                       eth_amount_size,
                       WEI_TO_ETHER,
                       msg->network_ticker,
                       msg->msg,
                       msg->msgLength);
        return;
    }

    amountToString(context->input_amount,
                   sizeof(context->input_amount),
                   context->input_decimals,
                   context->input_ticker,
                   msg->msg,
                   msg->msgLength);
}

// Set UI for "Receive" screen.
static void set_receive_ui(ethQueryContractUI_t *msg, const context_t *context) {
    strlcpy(msg->title, "Receive Min.", msg->titleLength);
    // set network ticker (ETH, BNB, etc) if needed
    if (ADDRESS_IS_NETWORK_TOKEN(context->output_token)) {
        strlcpy(context->output_ticker, msg->network_ticker, sizeof(context->output_ticker));
    }

    amountToString(context->min_output_amount,
                   sizeof(context->min_output_amount),
                   context->output_decimals,
                   context->output_ticker,
                   msg->msg,
                   msg->msgLength);
}

// Set UI for "Warning" screen.
static void set_warning_ui(ethQueryContractUI_t *msg,
                           const context_t *context __attribute__((unused))) {
    strlcpy(msg->title, "WARNING", msg->titleLength);
    strlcpy(msg->msg, "Unknown token", msg->msgLength);
}

// Helper function that returns the enum corresponding to the screen that should
// be displayed.
static screens_t get_screen(const ethQueryContractUI_t *msg, const context_t *context) {
    uint8_t index = msg->screenIndex;

    bool token_sent_found = context->token_found & TOKEN_SENT_FOUND;
    bool token_received_found = context->token_found & TOKEN_RECEIVED_FOUND;

    bool both_tokens_found = token_received_found && token_sent_found;
    bool both_tokens_not_found = !token_received_found && !token_sent_found;

    switch (index) {
        case 0:
            if (both_tokens_found) {
                return SEND_SCREEN;
            } else if (both_tokens_not_found) {
                return WARN_SCREEN;
            } else if (token_sent_found) {
                return SEND_SCREEN;
            } else if (token_received_found) {
                return WARN_SCREEN;
            }
            break;
        case 1:
            if (both_tokens_found) {
                return RECEIVE_SCREEN;
            } else if (both_tokens_not_found) {
                return SEND_SCREEN;
            } else if (token_sent_found) {
                return WARN_SCREEN;
            } else if (token_received_found) {
                return SEND_SCREEN;
            }
            break;
        case 2:
            if (both_tokens_not_found) {
                return WARN_SCREEN;
            } else {
                return RECEIVE_SCREEN;
            }
            break;
        case 3:
            if (both_tokens_found) {
                return ERROR;
            } else {
                return RECEIVE_SCREEN;
            }
            break;
        default:
            return ERROR;
            break;
    }
    return ERROR;
}

void handle_query_contract_ui(void *parameters) {
    ethQueryContractUI_t *msg = (ethQueryContractUI_t *) parameters;
    context_t *context = (context_t *) msg->pluginContext;

    // msg->title is the upper line displayed on the device.
    // msg->msg is the lower line displayed on the device.

    // Clean the display fields.
    memset(msg->title, 0, msg->titleLength);
    memset(msg->msg, 0, msg->msgLength);

    msg->result = ETH_PLUGIN_RESULT_OK;

    screens_t screen = get_screen(msg, context);

    switch (screen) {
        case SEND_SCREEN:
            set_send_ui(msg, context);
            break;
        case RECEIVE_SCREEN:
            set_receive_ui(msg, context);
            break;
        case WARN_SCREEN:
            set_warning_ui(msg, context);
            break;
        default:
            PRINTF("Received an invalid screenIndex\n");
            msg->result = ETH_PLUGIN_RESULT_ERROR;
            return;
    }
}
