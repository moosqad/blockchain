$(document).ready(() => {
  // Handle form submission to add transactions
  $("#add-transaction-form").submit((event) => {
    event.preventDefault();

    const sender = $("#sender-input").val();
    const receiver = $("#receiver-input").val();
    const amount = $("#amount-input").val();
    const signature = $("#signature-input").val();

    const transaction = {
      sender: sender,
      receiver: receiver,
      amount: Number(amount),
      signature: signature,
    };

    $.post(
      "http://localhost:8080",
      JSON.stringify(transaction),
      () => {
        alert("Transaction added to the blockchain.");
      }
    );
  });

  // Handle button click to get the blockchain
  $("#get-blockchain-button").click(() => {
    $.get("http://localhost:8080/get_blockchain", (data) => {
      $("#blockchain-data").text(data);
    });
  });
});

