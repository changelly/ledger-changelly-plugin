import 'regenerator-runtime/runtime';
import { waitForAppScreen, zemu, genericTx, nano_models } from './test.fixture';
import { BigNumber, ethers } from 'ethers';

const contractAddr = '0x1aaad07998466cd3eb8140827dddb37570be1e63';
const pluginName = 'changelly';
const testNetwork = 'ethereum';
const abi_path = `../networks/${testNetwork}/${pluginName}/abis/` + contractAddr + '.json';
const abi = require(abi_path);

nano_models.forEach(function (model) {
  test(
    '[Nano ' + model.letter + '] Sell to liquidity provider',
    zemu(model, async (sim, eth) => {
      const contract = new ethers.Contract(contractAddr, abi);

      // Constants used to create the transaction
      const WETH = '0xc02aaa39b223fe8d0a0e5c4f27ead9083c756cc2';
      const SUSHI = '0x6b3595068778dd592e39a122f4f5a5cf09c90fe2';
      const RECIPIENT = '0xEC89F1E142DFBDDbe30487fF5026c7427023e982';
      const MIN_BUY_AMOUNT = BigNumber.from('3000000');
      const ONE_ETH = BigNumber.from('100000000000000000');

      // signature: sellToLiquidityProvider(address,address,address,address,uint256,uint256,bytes)
      const { data } = await contract.populateTransaction.sellToLiquidityProvider(
        WETH,
        SUSHI,
        '0x6B3595068778DD592e39A122f4f5a5cF09C90fE2',
        RECIPIENT,
        ONE_ETH,
        MIN_BUY_AMOUNT,
        '0x1234'
      );

      // Get the generic transaction template
      let unsignedTx = genericTx;
      // Modify `to` to make it interact with the contract
      unsignedTx.to = contractAddr;
      // Modify the attached data
      unsignedTx.data = data;

      // Create serializedTx and remove the "0x" prefix
      const serializedTx = ethers.utils.serializeTransaction(unsignedTx).slice(2);

      const tx = eth.signTransaction("44'/60'/0'/0", serializedTx);

      const right_clicks = model.letter === 'S' ? 6 : 5;

      // Wait for the application to actually load and parse the transaction
      await waitForAppScreen(sim);
      // Navigate the display by pressing the right button `right_clicks` times, then pressing both buttons to accept the transaction.
      await sim.navigateAndCompareSnapshots('.', model.name + 'sell-to-liquidity-provider', [
        right_clicks,
        0,
      ]);

      await tx;
    })
  );
});
