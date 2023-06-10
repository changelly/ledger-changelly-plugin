import { ethers } from 'ethers';
import 'regenerator-runtime/runtime';
import { waitForAppScreen, zemu, nano_models, genericTx } from './test.fixture';

const contractAddr = '0x1aaad07998466cd3eb8140827dddb37570be1e63';
const chainID = 1; // Ethereum

nano_models.forEach(function (model) {
  test(
    '[Nano ' + model.letter + '] sail_uniswap_v3_swap',
    zemu(model, async (sim, eth) => {
      //SailUniswapV3Swap 0x1b328275
      const inputData =
        '0x1b3282750000000000000000000000000000000000000000000000000de0b6b3a764000000000000000000000000000000000000000000000000002c821876bad4a1aee30000000000000000000000000000000000000000000000000000000000000060000000000000000000000000000000000000000000000000000000000000000180000000000000000000000060594a405d53811d3bc4766596efd80fd545a270';

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
      const right_clicks = model.letter === 'S' ? 8 : 7;

      // Wait for the application to actually load and parse the transaction
      await waitForAppScreen(sim);
      // Navigate the display by pressing the right button `right_clicks` times, then pressing both buttons to accept the transaction.
      await sim.navigateAndCompareSnapshots('.', model.name + 'sail_uniswap_v3_swap', [right_clicks, 0]);

      await tx;
    })
  );
});
