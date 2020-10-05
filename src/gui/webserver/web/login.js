/**
 * Trigger button click by pressing enter (login) and set focus to first input
 * field.
 */
window.onload = function() {
  document.getElementById("l_username").focus();
  var l_input_pw = document.getElementById("l_password");
  l_input_pw.addEventListener("keyup", function(event) {
    if (event.keyCode === 13) {
      event.preventDefault();
      document.getElementById("l_btn").click();
    }
  });
}

/**
 * Send request to server, load overview-page if login is successfull.
 */
function DoLogin() {
  var json_request = new Object();
  json_request["username"] = document.getElementById("l_username").value;
  json_request["password"] = document.getElementById("l_password").value;
  
  var xhttp = new XMLHttpRequest();
  xhttp.open("POST", "/api/user_login");
  xhttp.send(JSON.stringify(json_request));

  xhttp.onload = function(event){
    if (xhttp.status==401) {
      console.log("login failed!");
      document.getElementById("center_box").style="border: 3px solid red;";
      document.getElementById("l_password").value="";
      document.getElementById("login_error").style="display: block;";
    }
    else {
      window.location="/overview";
    }
  };
}


