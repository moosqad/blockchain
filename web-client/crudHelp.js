
const helpTable = document.getElementById("helpTable").getElementsByTagName("tbody")[0];
const updateForm = document.getElementById("updateForm");
const form = updateForm.querySelector('form');
const recordIdInput = form.querySelector('input[name="id"]');
const firstNameInput = form.querySelector('input[name="firstName"]');
const secondNameInput = form.querySelector('input[name="secondName"]');
const thirdNameInput = form.querySelector('input[name="thirdName"]');
const problemInput = form.querySelector('input[name="problem"]');
const moneyGoalInput = form.querySelector('input[name="moneyGoal"]');
const filialIdInput = form.querySelector('input[name="filialId"]');

let recordToUpdate;

// Function to create a new row in the table with the given data
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

  // Add an 'UPDATE' button to the row
  const updateButton = document.createElement('button');
  updateButton.textContent = 'Отредактровать';
  updateButton.addEventListener('click', (event) => {
    // event.preventDefault(); // Prevent the form from being submitted
    console.log('Update button clicked');

    // Show the modal and fill the form with the data from the row
    recordToUpdate = {
      id: id,
      firstName: firstName,
      secondName: secondName,
      thirdName: thirdName,
      problem: problem,
      moneyGoal: moneyGoal,
      filialId: filialId
    };
    showUpdateModal(recordToUpdate);
  });
  newRow.insertCell().appendChild(updateButton);

  // Add a 'DELETE' button to the row
  const deleteButton = document.createElement('button');
  deleteButton.textContent = 'Удалить';
  deleteButton.addEventListener('click', (event) => {
    event.stopPropagation(); // Prevent the update button from being clicked
    console.log('Delete button clicked');

    // Delete the record from the server and remove the row from the table
    deleteRecord(id);
  });
  newRow.insertCell().appendChild(deleteButton);
}

function deleteRecord(id) {
  const data = {
    id: Number(id),
  }
  console.log(JSON.stringify(data));
  $.post('http://localhost:8000/delete', JSON.stringify(data), (data) => {
    console.log(response.data);
    const rowToDelete = helpTable.querySelector(`tr:nth-child(${id + 1})`);
    rowToDelete.remove();
    loadData();
  })
  location.reload();
}

// Function to show the update modal and fill the form with the data
function showUpdateModal(record) {
  // Fill the form with the data
  recordIdInput.value = record.id;
  firstNameInput.value = record.firstName;
  secondNameInput.value = record.secondName;
  thirdNameInput.value = record.thirdName;
  problemInput.value = record.problem;
  moneyGoalInput.value = record.moneyGoal;
  filialIdInput.value = record.filialId;

  // Show the modal
  $('#updateForm').modal({
    backdrop: false
  });


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

function handleFormSubmit(event) {
  event.preventDefault();
  const formData = new FormData(form);
  const id = formData.get('id');
  const firstName = formData.get('firstName');
  const secondName = formData.get('secondName');
  const thirdName = formData.get('thirdName');
  const problem = formData.get('problem');
  const moneyGoal = formData.get('moneyGoal');
  const filialId = formData.get('filialId');

  const update_data = {
    id: Number(id),
    first_name: firstName,
    second_name: secondName,
    third_name: thirdName,
    problem: problem,
    money_goal: Number(moneyGoal),
    filial_id: Number(filialId),
  };
  console.log(JSON.stringify(update_data));
  $.post('http://localhost:8000/update', JSON.stringify(update_data), (data) => {
    console.log(response);
    // Update the table row with the new data
    const rowToUpdate = helpTable.querySelector(`tr:nth-child(${recordToUpdate.id + 1})`);
    rowToUpdate.cells[1].textContent = firstName;
    rowToUpdate.cells[2].textContent = secondName;
    rowToUpdate.cells[3].textContent = thirdName;
    rowToUpdate.cells[4].textContent = problem;
    rowToUpdate.cells[5].textContent = moneyGoal;
    rowToUpdate.cells[6].textContent = filialId;
    // Hide the update form
    updateForm.style.display = "none";

    // Reload the data from the server and repopulate the table
    loadData();
  });
  location.reload();
}


// // Add a submit event listener to the form
form.addEventListener('submit', handleFormSubmit);

// Call the loadData function when the page loads
window.addEventListener("load", loadData);