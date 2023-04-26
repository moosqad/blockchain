$(document).ready(() => {
  const cookies = document.cookie.split(';');
  const usernameCookie = cookies.find(cookie => cookie.trim().startsWith('username='));
  const username = usernameCookie ? usernameCookie.split('=')[1] : null;
  // Set cookie with expiration time of 1 day
  const expirationTime = new Date();
  expirationTime.setDate(expirationTime.getDate() + 1);
  document.cookie = `expires=${expirationTime.toUTCString()};`;


  function printBlockchain(blockchain) {
    const table = document.getElementById("blockchain-table");
    table.innerHTML = ""; // Clear the table contents

    const headers = ["ID", "Хеш", "Предыдущий Хеш", "Время создания", "Перевод"];

    // Create the table header
    const thead = document.createElement("thead");
    const headerRow = document.createElement("tr");
    headers.forEach(header => {
      const headerCell = document.createElement("th");
      headerCell.textContent = header;
      headerRow.appendChild(headerCell);
    });
    thead.appendChild(headerRow);
    table.appendChild(thead);

    // Create the table body
    const tbody = document.createElement("tbody");
    blockchain.forEach(block => {
      const row = document.createElement("tr");
      row.classList.add("separator-bottom");

      const blockCell = document.createElement("td");
      blockCell.textContent = block.index;
      row.appendChild(blockCell);

      const hashCell = document.createElement("td");
      hashCell.textContent = block.hash;
      row.appendChild(hashCell);

      const prevHashCell = document.createElement("td");
      prevHashCell.textContent = block.previous_hash;
      row.appendChild(prevHashCell);

      const timestampCell = document.createElement("td");
      timestampCell.textContent = new Date(block.timestamp * 1000).toLocaleString();
      row.appendChild(timestampCell);

      const transactionsCell = document.createElement("td");
      const transactionsTable = document.createElement("table");
      transactionsTable.classList.add("table", "table-striped", "table-bordered");
      transactionsTable.style.width = "100%";
      transactionsTable.style.maxWidth = "600px";
      transactionsTable.style.margin = "auto";
      block.transactions.forEach((transaction, index) => {
        const transactionRow = document.createElement("tr");
        if (index < block.transactions.length - 1) {
          transactionRow.classList.add("separator-bottom");
        }

        const senderCell = document.createElement("td");
        senderCell.textContent = `{USER:${transaction.sender}}`;
        transactionRow.appendChild(senderCell);

        const arrow = document.createElement("td");
        arrow.textContent = " → ";
        arrow.style.fontWeight = "bold";
        arrow.style.fontSize = "20px";
        transactionRow.appendChild(arrow);

        const receiverCell = document.createElement("td");
        receiverCell.textContent = `{ID:${transaction.receiver}}`;
        transactionRow.appendChild(receiverCell);

        const dot = document.createElement("td");
        dot.textContent = " перевёл ";
        transactionRow.appendChild(dot);

        const amountCell = document.createElement("td");
        amountCell.textContent = transaction.amount + " руб.";
        transactionRow.appendChild(amountCell);

        transactionsTable.appendChild(transactionRow);
      });
      transactionsCell.appendChild(transactionsTable);
      row.appendChild(transactionsCell);

      tbody.appendChild(row);
    });
    table.appendChild(tbody);

    // Add separators between rows and columns
    const rows = table.getElementsByTagName("tr");
    for (let i = 0; i < rows.length; i++) {
      const cells = rows[i].getElementsByTagName("td");
      for (let j = 0; j < cells.length; j++) {
        cells[j].classList.add("separator-right");
      }
    }
  };

  $.ajax({
    url: "http://localhost:8080/get_blockchain",
    dataType: "json",
    success: function (response) {
      printBlockchain(response);
      // displayBlockchain(response);

    },
    error: function (jqXHR, textStatus, errorThrown) {
      console.log("Error:", errorThrown);
    },
  });



  $.ajax({
    url: "http://localhost:8080/is_valid",
    dataType: "json",
    success: function (response) {
      console.log(response.message);
      $("#blockchain-data").text(response.message);
      if (response.is_valid === true) {
        $("#validity").css("background-color", "green");
      } else {
        $("#validity").css("background-color", "red");
      }
    },
    error: function (jqXHR, textStatus, errorThrown) {
      console.log("Error:", errorThrown);
    },
  });

});

