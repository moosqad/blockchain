
const helpTable = document.getElementById("helpTable").getElementsByTagName("tbody")[0];
const updateForm = document.getElementById("updateForm");
const formContainer = document.querySelector('.container');
const form = updateForm.querySelector('form');
const recordIdInput = form.querySelector('input[name="id"]');
const firstNameInput = form.querySelector('input[name="firstName"]');
const secondNameInput = form.querySelector('input[name="secondName"]');
const thirdNameInput = form.querySelector('input[name="thirdName"]');
const problemInput = form.querySelector('input[name="problem"]');
const moneyGoalInput = form.querySelector('input[name="moneyGoal"]');
const filialIdInput = form.querySelector('input[name="filialId"]');

// Function to open the update form
function openUpdateForm() {
  updateForm.style.display = 'flex';
  formContainer.style.height = '100%';
}

// Function to close the update form
function closeUpdateForm() {
  updateForm.style.display = 'none';
  formContainer.style.height = 'auto';
}

let recordToUpdate;

const cookies = document.cookie.split(';');
const usernameCookie = cookies.find(cookie => cookie.trim().startsWith('username='));
const username = usernameCookie ? usernameCookie.split('=')[1] : null;

const first_nameCookie = cookies.find(cookie => cookie.trim().startsWith('first_name='));
const first_name = first_nameCookie ? first_nameCookie.split('=')[1] : null;

const second_nameCookie = cookies.find(cookie => cookie.trim().startsWith('second_name='));
const second_name = second_nameCookie ? second_nameCookie.split('=')[1] : null;

const third_nameCookie = cookies.find(cookie => cookie.trim().startsWith('third_name='));
const third_name = third_nameCookie ? third_nameCookie.split('=')[1] : null;


const phone_numberCookie = cookies.find(cookie => cookie.trim().startsWith('phone_number='));
const phone_number = phone_numberCookie ? phone_numberCookie.split('=')[1] : null;

const emailCookie = cookies.find(cookie => cookie.trim().startsWith('email='));
const email = emailCookie ? emailCookie.split('=')[1] : null;

const balanceCookie = cookies.find(cookie => cookie.trim().startsWith('balance='));
const balance = balanceCookie ? balanceCookie.split('=')[1] : null;

if (username == 'admin') {
  $(".nav-adm").text("Панель администратора");
}
$(".nav-username").text(`Пользователь: ${username}`);


function addRow(id, firstName, secondName, thirdName, problem, moneyGoal, already, filialId) {
  const newRow = helpTable.insertRow();
  newRow.insertCell().appendChild(document.createTextNode(id));
  newRow.insertCell().appendChild(document.createTextNode(firstName));
  newRow.insertCell().appendChild(document.createTextNode(secondName));
  newRow.insertCell().appendChild(document.createTextNode(thirdName));
  newRow.insertCell().appendChild(document.createTextNode(problem));
  newRow.insertCell().appendChild(document.createTextNode(moneyGoal));
  newRow.insertCell().appendChild(document.createTextNode(already));
  newRow.insertCell().appendChild(document.createTextNode(filialId));



  // Add an 'UPDATE' button to the row
  const updateButton = document.createElement('button');
  updateButton.textContent = 'Пожертвовать';
  updateButton.classList.add('button-primary');
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
    };
    showUpdateModal(recordToUpdate);

  });
  newRow.insertCell().appendChild(updateButton);

}


function get_balance(username) {
  const get_balance = { username: username, }
  $.post('http://localhost:8000/get_balance', JSON.stringify(get_balance), (data) => {
    // $(".nav-balance").text(`Баланс: ${data.balance}₽`);
    document.cookie = `balance=${data.balance}`;
    $(".nav-balance").text(`Баланс: ${data.balance}₽`);

  });

}


// Function to show the update modal and fill the form with the data
function showUpdateModal(record) {
  // Fill the form with the data
  recordIdInput.value = record.id;
  firstNameInput.value = record.firstName;
  secondNameInput.value = record.secondName;
  thirdNameInput.value = record.thirdName;
  moneyGoalInput.value = record.moneyGoal;

  openUpdateForm();


}

function already_got(receiver, callback) {
  const post = {
    receiver: receiver.toString(),
  };
  console.log(post);
  $.post('http://localhost:8080/get_sum', JSON.stringify(post), (data) => {
    console.log("response", data.response);
    callback(data.response.toString());
  });
}


// Function to populate the table with data from the Help class
function loadData() {
  axios.get('http://localhost:8000/read')
    .then((response) => {
      const data = response.data;
      data.forEach((row) => {
        already_got(row.id, (already) => {
          addRow(row.id, row.first_name, row.second_name, row.third_name, row.problem, row.money_goal, already, row.filial_id);
        });
      });
    })
    .catch((error) => {
      console.error(error);
    });
}


function updateBalance(amount, username) {

  const update_balance = {
    amount: Number(amount),
    username: username,
  }
  $.post('http://localhost:8000/update_balance', JSON.stringify(update_balance), (data) => {
    console.log(data);
  });
}

function get_user_info(username) {
  const user = { username: username, }
  $.post('http://localhost:8000/get_user_info', JSON.stringify(user), (data) => {

    document.cookie = `first_name=${data.first_name}`;
    document.cookie = `second_name=${data.second_name}`;
    document.cookie = `third_name=${data.third_name}`;
    document.cookie = `phone_number=${data.phone_number}`;
    document.cookie = `email=${data.email}`;
    console.log(data);

  });
}


function handleFormSubmit(event) {
  event.preventDefault();

  const formData = new FormData(form);
  const id = formData.get('id');

  const amount = formData.get('moneyGoal');


  const update_data = {
    sender: username,
    receiver: id,
    amount: Number(amount),
  };
  // console.log(JSON.stringify(update_data));
  if (Number(balance) > amount) {
    $.post('http://localhost:8080/add_transaction', JSON.stringify(update_data), (data) => {
      console.log(response);
      updateForm.style.display = "none";
    });
    updateBalance(amount, username);
    location.reload();
  }
  else {
    $("#status").text("Недостаточно средств!");
  }
}

console.log(username);
console.log(first_name);
console.log(second_name);
console.log(third_name);
console.log(phone_number);
console.log(email);

// // Add a submit event listener to the form
form.addEventListener('submit', handleFormSubmit);

// Call the loadData function when the page loads
window.addEventListener("load", loadData, get_balance(username), get_user_info(username));