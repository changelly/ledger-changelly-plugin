
import { ethers } from 'ethers';
import { parseEther } from 'ethers/lib/utils';
import 'regenerator-runtime/runtime';
import { waitForAppScreen, zemu, nano_models, genericTx } from './test.fixture';

const contractAddr = '0x8e4005c5a2f85408a95adf7831f9959eda7d87d1';
const chainID = 137; // Polygon

const pluginName = 'changelly';
const testNetwork = 'polygon';
const abi_path = `../networks/${testNetwork}/${pluginName}/abis/` + contractAddr + '.json';
const abi = require(abi_path);

nano_models.forEach(function (model) {
  test(
    '[Nano ' + model.letter + '] sail_adapter_swap_polygon',
    zemu(
      model,
      async (sim, eth) => {
        // sailAdapterSwap 0x1342555a
        const inputData =
          '0x1342555a0000000000000000000000001111111254eeb25477b68fb85ed929f73a96058200000000000000000000000000000000000000000000000000000000000000e0000000000000000000000000c2132d05d31c914a87c6611c10748aeb04b58e8f0000000000000000000000000000000000000000000000000000000005f5e1000000000000000000000000002791bca1f2de4661ed88a30c99a7a9449aa841740000000000000000000000000000000000000000000000000000000005f3ad24000000000000000000000000026e0c5638ae22d4e6b583c996172c65c0a75a5a00000000000000000000000000000000000000000000000000000000000001e812aa3caf000000000000000000000000b97cd69145e5a9357b2acd6af6c5076380f17afb000000000000000000000000c2132d05d31c914a87c6611c10748aeb04b58e8f0000000000000000000000002791bca1f2de4661ed88a30c99a7a9449aa84174000000000000000000000000cdc878c037625afe3a98e14fcc56e169f0b5b41100000000000000000000000072fa2c72cefa2cd2d49580f6eafb1b9ec5d8a4fc0000000000000000000000000000000000000000000000000000000005f5e1000000000000000000000000000000000000000000000000000000000005f3ad240000000000000000000000000000000000000000000000000000000000000004000000000000000000000000000000000000000000000000000000000000014000000000000000000000000000000000000000000000000000000000000001600000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000005800000000000000000000000000000000000000000000000000000000003a4020cdc878c037625afe3a98e14fcc56e169f0b5b411bd6015b40000000000000000000000001111111254eeb25477b68fb85ed929f73a9605820000000000000000cfee7c08000000000000000000000000000000000000000000000000d3da1b6465320cb4f3';

        // Get the generic transaction template
        let unsignedTx = genericTx;
        // Adapt to the appropriate network
        unsignedTx.chainId = chainID;
        // Modify `to` to make it interact with the contract
        unsignedTx.to = contractAddr;
        // Modify the attached data
        unsignedTx.data = inputData;

        // Create serializedTx and remove the "0x" prefix
        const serializedTx = ethers.utils.serializeTransaction(unsignedTx).slice(2);
        const tx = eth.signTransaction("44'/60'/0'/0", serializedTx);
        const right_clicks = model.letter === 'S' ? 9 : 7;

        // Wait for the application to actually load and parse the transaction
        await waitForAppScreen(sim);
        // Navigate the display by pressing the right button `right_clicks` times, then pressing both buttons to accept the transaction.
        await sim.navigateAndCompareSnapshots('.', model.name + 'sail_adapter_swap_polygon', [
          right_clicks,
          0,
        ]);

        await tx;
      },
      'polygon'
    )
  );
});
