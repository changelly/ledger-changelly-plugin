import 'core-js/stable';
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
    '[Nano ' + model.letter + '] agg_router_swap_polygon',
    zemu(
      model,
      async (sim, eth) => {
        // aggRouterSwap 0x97b3c273
        const inputData =
          '0x97b3c27300000000000000000000000000000000000000000000000000000000000000a0000000000000000000000000eeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeee00000000000000000000000000000000000000000000000000038d7ea4c68000000000000000000000000000fdc26cda2d2440d0e83cd1dee8e8be48405806dc000000000000000000000000000000000000000000000000001c163c0f190a9f00000000000000000000000000000000000000000000000000000000000002e812aa3caf0000000000000000000000000d15038f8a0362b4ce71d6c879d56bf9fc2884cf000000000000000000000000eeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeee000000000000000000000000fdc26cda2d2440d0e83cd1dee8e8be48405806dc0000000000000000000000000d15038f8a0362b4ce71d6c879d56bf9fc2884cf0000000000000000000000002408126d6384c42b5e03e3ff3bd9adee519ea6aa00000000000000000000000000000000000000000000000000038d7ea4c68000000000000000000000000000000000000000000000000000001c163c0f190a9f0000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000014000000000000000000000000000000000000000000000000000000000000001600000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000015200000000000000000000000000000000000000000000000000000000013400a007e5c0d20000000000000000000000000000000000000000000001100000a900001a40410d500b1d8e8ef31e21c99d1db9a6444d3adf1270d0e30db00c200d500b1d8e8ef31e21c99d1db9a6444d3adf1270adbf1854e5883eb8aa7baf50705338739e558e5b6ae4071138002dc6c0adbf1854e5883eb8aa7baf50705338739e558e5becc89c265abe55376e098a081c8144d4e407a82d000000000000000000000000000000000000000000000000000000a86d52f00d0d500b1d8e8ef31e21c99d1db9a6444d3adf127000206ae4071138002dc6c0ecc89c265abe55376e098a081c8144d4e407a82d1111111254eeb25477b68fb85ed929f73a960582000000000000000000000000000000000000000000000000001c163c0f190a9f7ceb23fd6bc0add59e62ac25578270cff1b9f6190000000000000000000000000000cfee7c08000000000000000000000000000000000000000000000000cee13d63773629';

        // Get the generic transaction template
        let unsignedTx = genericTx;
        //adapt to the appropriate network
        unsignedTx.chainId = chainID;
        // Modify `to` to make it interact with the contract
        unsignedTx.to = contractAddr;
        // Modify the attached data
        unsignedTx.data = inputData;
        // Modify the number of MATIC sent
        unsignedTx.value = parseEther('0.001');

        // Create serializedTx and remove the "0x" prefix
        const serializedTx = ethers.utils.serializeTransaction(unsignedTx).slice(2);
        const tx = eth.signTransaction("44'/60'/0'/0", serializedTx);
        const right_clicks = model.letter === 'S' ? 8 : 6;

        // Wait for the application to actually load and parse the transaction
        await waitForAppScreen(sim);
        // Navigate the display by pressing the right button `right_clicks` times, then pressing both buttons to accept the transaction.
        await sim.navigateAndCompareSnapshots('.', model.name + 'agg_router_swap_polygon', [right_clicks, 0]);

        await tx;
      },
      'polygon'
    )
  );
});
