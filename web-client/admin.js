$(document).ready(() => {
  const cookies = document.cookie.split(';');
  const usernameCookie = cookies.find(cookie => cookie.trim().startsWith('username='));
  const username = usernameCookie ? usernameCookie.split('=')[1] : null;

  if (username !== "admin") {
    window.location.href = "registration.html";
  }

});
