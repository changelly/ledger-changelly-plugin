import 'core-js/stable';
import { BigNumber, ethers } from 'ethers';
import { parseEther } from 'ethers/lib/utils';
import 'regenerator-runtime/runtime';
import { waitForAppScreen, zemu, nano_models, genericTx } from './test.fixture';

const contractAddr = '0x1aaad07998466cd3eb8140827dddb37570be1e63';
const pluginName = 'changelly';
const testNetwork = 'ethereum';
const abi_path = `../networks/${testNetwork}/${pluginName}/abis/` + contractAddr + '.json';
const abi = require(abi_path);

nano_models.forEach(function (model) {
  test(
    '[Nano ' + model.letter + '] Sell ETH for token uniswap',
    zemu(model, async (sim, eth) => {
      const contract = new ethers.Contract(contractAddr, abi);

      // Constants used to create the transaction
      const RECIPIENT = '0xEC89F1E142DFBDDbe30487fF5026c7427023e982';
      const MIN_BUY_AMOUNT = BigNumber.from('133701200000000000');
      const OFFSET = '0x0000000000000000000000000000000000000000000000000000000000000060';

      // signature: sellToLiquidityProvider(address,address,address,address,uint256,uint256,bytes)
      const { data } = await contract.populateTransaction.sellEthForTokenToUniswapV3(
        OFFSET,
        MIN_BUY_AMOUNT,
        RECIPIENT
      );

      // Get the generic transaction template
      let unsignedTx = genericTx;
      // Modify `to` to make it interact with the contract
      unsignedTx.to = contractAddr;
      // Modify the attached data
      unsignedTx.data = data;
      // Add Ether
      unsignedTx.value = parseEther('0.2722');

      // Create serializedTx and remove the "0x" prefix
      const serializedTx = ethers.utils.serializeTransaction(unsignedTx).slice(2);

      const tx = eth.signTransaction("44'/60'/0'/0", serializedTx);

      const right_clicks = 6;

      // Wait for the application to actually load and parse the transaction
      await waitForAppScreen(sim);
      // Navigate the display by pressing the right button `right_clicks` times, then pressing both buttons to accept the transaction.
      await sim.navigateAndCompareSnapshots('.', model.name + 'sell-eth-for-token-uniswap', [
        right_clicks,
        0,
      ]);

      await tx;
    })
  );
});
