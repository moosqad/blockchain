$(document).ready(() => {
  $("#sign-in").click(() => {
    console.log("clicked");
    const username = $("#username").val();
    const password = $("#password").val();

    const new_user = {
      username: username,
      password: password,
    };
    console.log(new_user);
    $.post(
      "http://localhost:8000/sign_in",
      JSON.stringify(new_user),
      (data) => {
        console.log(data.response);
        if (data.response == true) {
          document.cookie = `username=${username}`;

          window.location.href = "transactions.html";
        }
      }
    );
  });
});
