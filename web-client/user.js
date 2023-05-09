
function clearCookies() {
  // Get all the cookies
  const cookies = document.cookie.split(";");

  // Loop through each cookie and set its expiration date to a past date
  cookies.forEach((cookie) => {
    const cookieParts = cookie.split("=");
    const cookieName = cookieParts[0].trim();
    document.cookie = `${cookieName}=; expires=Thu, 01 Jan 1970 00:00:00 UTC; path=/;`;
  });
  location.reload();
}



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
  $(".adm-panel").text("Админ панель");
}
$(".nav-username").text(`Пользователь: ${username}`);



function get_balance(username) {
  const get_balance = { username: username, }
  $.post('http://localhost:8000/get_balance', JSON.stringify(get_balance), (data) => {
    // $(".nav-balance").text(`Баланс: ${data.balance}₽`);
    document.cookie = `balance=${data.balance}`;
    $(".nav-balance").text(`Баланс: ${data.balance}₽`);

  });

}

function get_user_info(username) {
  const user = { username: username, }
  $.post('http://localhost:8000/get_user_info', JSON.stringify(user), (data) => {
    document.cookie = `username=${data.username}`;
    document.cookie = `first_name=${data.first_name}`;
    document.cookie = `second_name=${data.second_name}`;
    document.cookie = `third_name=${data.third_name}`;
    document.cookie = `phone_number=${data.phone_number}`;
    document.cookie = `email=${data.email}`;
    console.log(data);

  });
}

// Call the loadData function when the page loads
window.addEventListener("load", get_balance(username), get_user_info(username));