// function get_user_info(username) {
//   const user = { username: username, }
//   $.post('http://localhost:8000/get_balance', JSON.stringify(user), (data) => {
//     document.cookie = `balance=${data.balance}`;
//     document.cookie = `first_name=${data.first_name}`;
//     document.cookie = `second_name=${data.second_name}`;
//     document.cookie = `third_name=${data.third_name}`;
//     document.cookie = `phone_number=${data.phone_number}`;
//     document.cookie = `email=${data.email}`;
//   });
// }

// // // Add a submit event listener to the form
// form.addEventListener('submit', handleFormSubmit);

// // Call the loadData function when the page loads
// window.addEventListener("load", loadData, get_user_info(username));