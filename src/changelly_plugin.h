#pragma once

#include <string.h>

#include "eth_internals.h"
#include "eth_plugin_interface.h"

// Number of selectors defined in this plugin. Should match the enum
// `selector_t`.
#define NUM_SELECTORS 18

// Name of the plugin.
#define PLUGIN_NAME "Changelly"

// Changelly uses `0xeeeee` as a dummy address to represent ETH.
extern const uint8_t NATIVE_TOKEN_ADDRESS[ADDRESS_LENGTH];

// Address 0x00000... used to indicate that the recipient is the sender.
extern const uint8_t NULL_ETH_ADDRESS[ADDRESS_LENGTH];

// Returns 1 if corresponding address is the Changelly address for the native chain
// token (ETH, BNB, MATIC, etc.. are 0xeeeee...).
#define ADDRESS_IS_NETWORK_TOKEN(_addr) (!memcmp(_addr, NATIVE_TOKEN_ADDRESS, ADDRESS_LENGTH))

#define TOKEN_SENT_FOUND     1
#define TOKEN_RECEIVED_FOUND 1 << 1

// Enumeration of the different selectors possible.
// Should follow the exact same order as the array declared in main.c
typedef enum {
    SELL_TO_UNISWAP = 0,
    SELL_TO_LIQUIDITY_PROVIDER,
    TRANSFORM_ERC20,
    SAIL_ADAPTER_SWAP,
    SAIL_ADAPTER_SWAP_WITH_FEE,
    SELL_ETH_FOR_TOKEN_TO_UNISWAP_V3,
    SELL_TOKEN_FOR_ETH_TO_UNISWAP_V3,
    SELL_TOKEN_FOR_TOKEN_TO_UNISWAP_V3,
    SELL_TO_PANCAKESWAP,
    SAIL_UNISWAP_V3_SWAP,
    SAIL_UNISWAP_V3_SWAP_WITH_FEE,
    SAIL_UNISWAP_V3_SWAP_WITH_SLIPPAGE,
    SAIL_UNISWAP_V3_SWAP_WITH_FEE_AND_SLIPPAGE,
    SAIL_SPLIT_UNISWAP_V3_SWAP,
    SAIL_SPLIT_UNISWAP_V3_SWAP_WITH_FEE,
    SAIL_SPLIT_UNISWAP_V3_SWAP_WITH_SLIPPAGE,
    SAIL_SPLIT_UNISWAP_V3_SWAP_WITH_FEE_AND_SLIPPAGE,
    ANY_RECIPIENT_TRANSFORM_ERC20
} selector_t;

// Enumeration used to parse the smart contract data.
typedef enum {
    INPUT_TOKEN = 0,
    OUTPUT_TOKEN,
    INPUT_AMOUNT,
    MIN_OUTPUT_AMOUNT,
    SKIP,
    UNEXPECTED_PARAMETER,
    OFFSET,
    TARGET,
    RECIPIENT
} parameter_t;

extern const uint32_t CHANGELLY_SELECTORS[NUM_SELECTORS];

// Shared global memory with Ethereum app. Must be at most 5 * 32 bytes.
typedef struct context_t {
    // For display.
    uint8_t input_token[ADDRESS_LENGTH];
    uint8_t output_token[ADDRESS_LENGTH];
    uint8_t input_amount[INT256_LENGTH];
    uint8_t min_output_amount[INT256_LENGTH];
    char input_ticker[MAX_TICKER_LEN];
    char output_ticker[MAX_TICKER_LEN];
    uint8_t input_decimals;
    uint8_t output_decimals;
    char recipient[ADDRESS_LENGTH];
    bool recieve_screen_only;  // Display receive screen only (except warnings).

    uint8_t token_found;

    // For parsing data.
    uint8_t next_param;  // Set to be the next param we expect to parse.
    uint16_t offset;     // Offset at which the array or struct starts.
    bool go_to_offset;   // If set, will force the parsing to iterate through
                         // parameters until `offset` is reached.

    // For both parsing and display.
    selector_t selectorIndex;
} context_t;

typedef enum {
    SEND_SCREEN,
    RECEIVE_SCREEN,
    RECIPIENT_SCREEN,
    WARN_SCREEN,
    ERROR,
} screens_t;

// Number of decimals used when the token wasn't found in the CAL.
#define DEFAULT_DECIMAL WEI_TO_ETHER

// Ticker used when the token wasn't found in the CAL.
#define DEFAULT_TICKER ""

// Piece of code that will check that the above structure is not bigger than 5
// * 32. Do not remove this check.
_Static_assert(sizeof(context_t) <= 5 * 32, "Structure of parameters too big.");

void handle_provide_parameter(void *parameters);
void handle_query_contract_ui(void *parameters);
void handle_init_contract(void *parameters);
void handle_finalize(void *parameters);
void handle_provide_token(void *parameters);
void handle_query_contract_id(void *parameters);

static inline void printf_hex_array(const char *title __attribute__((unused)),
                                    size_t len __attribute__((unused)),
                                    const uint8_t *data __attribute__((unused))) {
    PRINTF(title);
    for (size_t i = 0; i < len; ++i) {
        PRINTF("%02x", data[i]);
    };
    PRINTF("\n");
}
