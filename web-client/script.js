$(document).ready(() => {
  console.log("ready!");

  $("#add-transaction-form").submit((event) => {
    event.preventDefault();

    const sender = $("#sender-input").val();
    const receiver = $("#receiver-input").val();
    const amount = $("#amount-input").val();

    const transaction = {
      sender: sender,
      receiver: receiver,
      amount: Number(amount),
    };

    $.post(
      "http://localhost:8080",
      JSON.stringify(transaction),
      () => {
        alert("Transaction added to the blockchain.");
      }
    );
  });

  function printBlockchain(blockchain) {
    for (let block of blockchain) {
      console.log(`Block ${block.index}:`);
      console.log(`Hash: ${block.hash}`);
      console.log(`Previous Hash: ${block.previous_hash}`);
      console.log(`Timestamp: ${block.timestamp}`);
      console.log(`Nonce: ${block.nonce}`);
      console.log(`Transactions:`);
      for (let transaction of block.transactions) {
        console.log(`Sender: ${transaction.sender}`);
        console.log(`Receiver: ${transaction.receiver}`);
        console.log(`Amount: ${transaction.amount}`);
        console.log(`Signature: ${transaction.signature}`);
      }
      console.log('\n');
    }
  };

  function printBlockchain(blockchain) {
    const table = document.getElementById("blockchain-table");
    table.innerHTML = ""; // Clear the table contents

    const headerRow = table.insertRow();
    const headers = ["Block", "Hash", "Previous Hash", "Timestamp", "Transactions"];

    // Add header cells to the header row
    for (let i = 0; i < headers.length; i++) {
      const headerCell = document.createElement("th");
      headerCell.innerText = headers[i];
      headerRow.appendChild(headerCell);
    }

    // Add a row for each block in the blockchain
    for (let i = 0; i < blockchain.length; i++) {
      const block = blockchain[i];
      const row = table.insertRow();

      const blockCell = row.insertCell();
      blockCell.innerText = block.index;

      const hashCell = row.insertCell();
      hashCell.innerText = block.hash;

      const prevHashCell = row.insertCell();
      prevHashCell.innerText = block.previous_hash;

      const timestampCell = row.insertCell();
      timestampCell.innerText = new Date(block.timestamp * 1000).toLocaleString();

      const transactionsCell = row.insertCell();
      const transactionsTable = document.createElement("table");
      for (let j = 0; j < block.transactions.length; j++) {
        const transaction = block.transactions[j];
        const transactionRow = transactionsTable.insertRow();

        const senderCell = transactionRow.insertCell();
        senderCell.innerText = transaction.sender;

        const arrow = transactionRow.insertCell();
        arrow.innerText = "->";

        const receiverCell = transactionRow.insertCell();
        receiverCell.innerText = transaction.receiver;

        const amountCell = transactionRow.insertCell();
        amountCell.innerText = transaction.amount;
      }
      transactionsCell.appendChild(transactionsTable);
    }
  }
  ;

  $("#get-blockchain-button").click(() => {
    $.ajax({
      url: "http://localhost:8080/get_blockchain",
      dataType: "json",
      success: function (response) {
        printBlockchain(response);
        displayBlockchain(response);

      },
      error: function (jqXHR, textStatus, errorThrown) {
        console.log("Error:", errorThrown);
      },
    });
  });


  $("#valid-blockchain-button").click(() => {
    $.ajax({
      url: "http://localhost:8080/is_valid",
      dataType: "json",
      success: function (response) {
        console.log(response.message);
        $("#blockchain-data").text(response.message);
      },
      error: function (jqXHR, textStatus, errorThrown) {
        console.log("Error:", errorThrown);
      },
    });
  });

});
