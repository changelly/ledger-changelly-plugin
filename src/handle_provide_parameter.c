#include "changelly_plugin.h"

static void handle_input_token(const ethPluginProvideParameter_t *msg, context_t *context) {
    memset(context->input_token, 0, sizeof(context->input_token));
    memcpy(context->input_token,
           &msg->parameter[PARAMETER_LENGTH - ADDRESS_LENGTH],
           sizeof(context->input_token));
    PRINTF("TOKEN SENT: %.*H\n", ADDRESS_LENGTH, context->input_token);
}

static void fill_input_token_as_eth(context_t *context) {
    memset(context->input_token, 0, sizeof(context->input_token));
    memcpy(context->input_token, NATIVE_TOKEN_ADDRESS, sizeof(context->input_token));
    PRINTF("TOKEN SENT: %.*H\n", ADDRESS_LENGTH, context->input_token);
}

static void handle_output_token(const ethPluginProvideParameter_t *msg, context_t *context) {
    memset(context->output_token, 0, sizeof(context->output_token));
    memcpy(context->output_token,
           &msg->parameter[PARAMETER_LENGTH - ADDRESS_LENGTH],
           sizeof(context->output_token));
    PRINTF("TOKEN RECEIVED: %.*H\n", ADDRESS_LENGTH, context->output_token);
}

static void fill_output_token_as_eth(context_t *context) {
    memset(context->output_token, 0, sizeof(context->output_token));
    memcpy(context->output_token, NATIVE_TOKEN_ADDRESS, sizeof(context->output_token));
    PRINTF("TOKEN RECEIVED: %.*H\n", ADDRESS_LENGTH, context->output_token);
}

static void handle_transform_erc20(ethPluginProvideParameter_t *msg, context_t *context) {
    switch (context->next_param) {
        case INPUT_TOKEN:
            handle_input_token(msg, context);
            context->next_param = OUTPUT_TOKEN;
            break;
        case OUTPUT_TOKEN:
            handle_output_token(msg, context);
            context->next_param = INPUT_AMOUNT;
            break;
        case INPUT_AMOUNT:
            copy_parameter(context->input_amount, msg->parameter, sizeof(context->input_amount));
            context->next_param = MIN_OUTPUT_AMOUNT;
            break;
        case MIN_OUTPUT_AMOUNT:
            copy_parameter(context->min_output_amount,
                           msg->parameter,
                           sizeof(context->min_output_amount));
            context->next_param = SKIP;
            break;
        case SKIP:
            break;
        default:
            PRINTF("Param not supported: %d\n", context->next_param);
            msg->result = ETH_PLUGIN_RESULT_ERROR;
            break;
    }
}

static void handle_sell_to_liquidity_provider(ethPluginProvideParameter_t *msg,
                                              context_t *context) {
    if (context->go_to_offset) {
        if (msg->parameterOffset != context->offset + SELECTOR_SIZE) {
            return;
        }
        context->go_to_offset = false;
    }

    switch (context->next_param) {
        case INPUT_TOKEN:
            handle_input_token(msg, context);
            context->next_param = OUTPUT_TOKEN;
            break;
        case OUTPUT_TOKEN:
            handle_output_token(msg, context);
            context->go_to_offset = true;
            context->offset = msg->parameterOffset - SELECTOR_SIZE + PARAMETER_LENGTH * 3;
            context->next_param = INPUT_AMOUNT;
            break;
        case INPUT_AMOUNT:
            copy_parameter(context->input_amount, msg->parameter, sizeof(context->input_amount));
            context->next_param = MIN_OUTPUT_AMOUNT;
            break;
        case MIN_OUTPUT_AMOUNT:
            copy_parameter(context->min_output_amount,
                           msg->parameter,
                           sizeof(context->min_output_amount));
            context->next_param = SKIP;
            break;
        case SKIP:
            break;
        default:
            PRINTF("Param not supported: %d\n", context->next_param);
            msg->result = ETH_PLUGIN_RESULT_ERROR;
            break;
    }
}

static void handle_agg_router_swap(ethPluginProvideParameter_t *msg, context_t *context) {
    switch (context->next_param) {
        case OFFSET:
            context->next_param = INPUT_TOKEN;
            break;
        case INPUT_TOKEN:
            handle_input_token(msg, context);
            context->next_param = INPUT_AMOUNT;
            break;
        case INPUT_AMOUNT:
            copy_parameter(context->input_amount, msg->parameter, sizeof(context->input_amount));
            context->next_param = OUTPUT_TOKEN;
            break;
        case OUTPUT_TOKEN:
            handle_output_token(msg, context);
            context->next_param = MIN_OUTPUT_AMOUNT;
            break;
        case MIN_OUTPUT_AMOUNT:
            copy_parameter(context->min_output_amount,
                           msg->parameter,
                           sizeof(context->min_output_amount));
            context->next_param = SKIP;
            break;
        case SKIP:
            break;
        default:
            PRINTF("Param not supported: %d\n", context->next_param);
            msg->result = ETH_PLUGIN_RESULT_ERROR;
            break;
    }
}

static void handle_sell_to_pancakeswap(ethPluginProvideParameter_t *msg, context_t *context) {
    if (context->go_to_offset) {
        if (msg->parameterOffset != context->offset + SELECTOR_SIZE) {
            return;
        }
        context->go_to_offset = false;
    }

    switch (context->next_param) {
        case OFFSET:
            context->next_param = INPUT_AMOUNT;
            break;
        case INPUT_AMOUNT:
            copy_parameter(context->input_amount, msg->parameter, sizeof(context->input_amount));
            context->next_param = MIN_OUTPUT_AMOUNT;
            break;
        case MIN_OUTPUT_AMOUNT:
            copy_parameter(context->min_output_amount,
                           msg->parameter,
                           sizeof(context->min_output_amount));
            context->go_to_offset = true;
            context->offset = msg->parameterOffset - SELECTOR_SIZE + PARAMETER_LENGTH * 3;
            context->next_param = INPUT_TOKEN;
            break;
        case INPUT_TOKEN:
            handle_input_token(msg, context);
            context->next_param = OUTPUT_TOKEN;
            break;
        case OUTPUT_TOKEN:
            handle_output_token(msg, context);
            context->next_param = SKIP;
            break;
        case SKIP:
            break;
        default:
            PRINTF("Param not supported: %d\n", context->next_param);
            msg->result = ETH_PLUGIN_RESULT_ERROR;
            break;
    }
}

static void handle_sell_to_uniswap(ethPluginProvideParameter_t *msg, context_t *context) {
    if (context->go_to_offset) {
        if (msg->parameterOffset != context->offset + SELECTOR_SIZE) {
            return;
        }
        context->go_to_offset = false;
    }

    switch (context->next_param) {
        case OFFSET:
            context->next_param = INPUT_AMOUNT;
            break;
        case INPUT_AMOUNT:
            copy_parameter(context->input_amount, msg->parameter, sizeof(context->input_amount));
            context->next_param = MIN_OUTPUT_AMOUNT;
            break;
        case MIN_OUTPUT_AMOUNT:
            copy_parameter(context->min_output_amount,
                           msg->parameter,
                           sizeof(context->min_output_amount));
            context->go_to_offset = true;
            context->offset = msg->parameterOffset - SELECTOR_SIZE + PARAMETER_LENGTH * 3;
            context->next_param = INPUT_TOKEN;
            break;
        case INPUT_TOKEN:
            handle_input_token(msg, context);
            context->next_param = OUTPUT_TOKEN;
            break;
        case OUTPUT_TOKEN:
            handle_output_token(msg, context);
            context->next_param = SKIP;
            break;
        case SKIP:
            break;
        default:
            PRINTF("Param not supported: %d\n", context->next_param);
            msg->result = ETH_PLUGIN_RESULT_ERROR;
            break;
    }
}

static void handle_sell_eth_for_token_to_uniswap_v3(ethPluginProvideParameter_t *msg,
                                                    context_t *context) {
    switch (context->next_param) {
        case OFFSET:
            fill_input_token_as_eth(context);
            context->next_param = MIN_OUTPUT_AMOUNT;
            break;
        case MIN_OUTPUT_AMOUNT:
            copy_parameter(context->min_output_amount,
                           msg->parameter,
                           sizeof(context->min_output_amount));
            context->next_param = SKIP;
            break;
        case SKIP:
            break;
        default:
            PRINTF("Param not supported: %d\n", context->next_param);
            msg->result = ETH_PLUGIN_RESULT_ERROR;
            break;
    }
}

static void handle_sell_token_for_eth_uniswap_v3(ethPluginProvideParameter_t *msg,
                                                 context_t *context) {
    switch (context->next_param) {
        case OFFSET:
            context->next_param = INPUT_AMOUNT;
            break;
        case INPUT_AMOUNT:
            copy_parameter(context->input_amount, msg->parameter, sizeof(context->input_amount));
            context->next_param = MIN_OUTPUT_AMOUNT;
            break;
        case MIN_OUTPUT_AMOUNT:
            fill_output_token_as_eth(context);
            copy_parameter(context->min_output_amount,
                           msg->parameter,
                           sizeof(context->min_output_amount));
            context->next_param = SKIP;
            break;
        case SKIP:
            break;
        default:
            PRINTF("Param not supported: %d\n", context->next_param);
            msg->result = ETH_PLUGIN_RESULT_ERROR;
            break;
    }
}

static void handle_sell_token_for_token_uniswap_v3(ethPluginProvideParameter_t *msg,
                                                   context_t *context) {
    switch (context->next_param) {
        case OFFSET:
            context->next_param = INPUT_AMOUNT;
            break;
        case INPUT_AMOUNT:
            copy_parameter(context->input_amount, msg->parameter, sizeof(context->input_amount));
            context->next_param = MIN_OUTPUT_AMOUNT;
            break;
        case MIN_OUTPUT_AMOUNT:
            copy_parameter(context->min_output_amount,
                           msg->parameter,
                           sizeof(context->min_output_amount));
            context->next_param = SKIP;
            break;
        case SKIP:
            break;
        default:
            PRINTF("Param not supported: %d\n", context->next_param);
            msg->result = ETH_PLUGIN_RESULT_ERROR;
            break;
    }
}

void handle_provide_parameter(void *parameters) {
    ethPluginProvideParameter_t *msg = (ethPluginProvideParameter_t *) parameters;
    context_t *context = (context_t *) msg->pluginContext;
    // We use `%.*H`: it's a utility function to print bytes. You first give
    // the number of bytes you wish to print (in this case, `PARAMETER_LENGTH`)
    // and then the address (here `msg->parameter`).
    PRINTF("Changelly plugin provide parameter: offset %d\nBytes: %.*H\n",
           msg->parameterOffset,
           PARAMETER_LENGTH,
           msg->parameter);

    msg->result = ETH_PLUGIN_RESULT_OK;

    switch (context->selectorIndex) {
        case TRANSFORM_ERC20:
            handle_transform_erc20(msg, context);
            break;
        case AGG_ROUTER_SWAP_WITH_FEE:
        case AGG_ROUTER_SWAP:
            handle_agg_router_swap(msg, context);
            break;
        case SELL_TO_UNISWAP:
            handle_sell_to_uniswap(msg, context);
            break;
        case SELL_TO_LIQUIDITY_PROVIDER:
            handle_sell_to_liquidity_provider(msg, context);
            break;
        case SELL_ETH_FOR_TOKEN_TO_UNISWAP_V3:
            handle_sell_eth_for_token_to_uniswap_v3(msg, context);
            break;
        case SELL_TOKEN_FOR_ETH_TO_UNISWAP_V3:
            handle_sell_token_for_eth_uniswap_v3(msg, context);
            break;
        case SELL_TOKEN_FOR_TOKEN_TO_UNISWAP_V3:
            handle_sell_token_for_token_uniswap_v3(msg, context);
            break;
        case SELL_TO_PANCAKESWAP:
            handle_sell_to_pancakeswap(msg, context);
            break;
        default:
            PRINTF("Selector Index not supported: %d\n", context->selectorIndex);
            msg->result = ETH_PLUGIN_RESULT_ERROR;
            break;
    }
}