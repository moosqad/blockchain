const helpTable = document.getElementById("helpTable").getElementsByTagName("tbody")[0];

// Function to create a new row in the table with the given data
function addRow(id, firstName, secondName, thirdName, problem, moneyGoal, filialId) {
  const newRow = helpTable.insertRow();
  newRow.insertCell().appendChild(document.createTextNode(id));
  newRow.insertCell().appendChild(document.createTextNode(firstName));
  newRow.insertCell().appendChild(document.createTextNode(secondName));
  newRow.insertCell().appendChild(document.createTextNode(thirdName));
  newRow.insertCell().appendChild(document.createTextNode(problem));
  newRow.insertCell().appendChild(document.createTextNode(moneyGoal));
  newRow.insertCell().appendChild(document.createTextNode(filialId));
}

// Function to populate the table with data from the Help class
function loadData() {
  axios.get('http://localhost:8000/read')
    .then((response) => {
      const data = response.data;
      data.forEach((row) => {
        addRow(row.id, row.first_name, row.second_name, row.third_name, row.problem, row.money_goal, row.filial_id);
      });
    })
    .catch((error) => {
      console.error(error);
    });
}

// Call the loadData function when the page loads
window.addEventListener("load", loadData);
