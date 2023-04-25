const createForm = document.getElementById('createHelpForm');

createForm.addEventListener('submit', (event) => {
  event.preventDefault();

  const formData = new FormData(createForm);
  const firstName = formData.get('firstName');
  const secondName = formData.get('secondName');
  const thirdName = formData.get('thirdName');
  const problem = formData.get('problem');
  const moneyGoal = formData.get('moneyGoal');
  const filialId = formData.get('filialId');

  const create_data = {
    first_name: firstName,
    second_name: secondName,
    third_name: thirdName,
    problem: problem,
    money_goal: Number(moneyGoal),
    filial_id: Number(filialId),
  };

  console.log(JSON.stringify(create_data));

  $.post("http://localhost:8000/create", JSON.stringify(create_data), (data) => {

  });
  window.location.href = "crudHelp.html";
});