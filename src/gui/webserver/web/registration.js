/**
 * Trigger button click by pressing enter, when on last input field and set
 * focus on first input field.
 */
window.onload = function() {
  document.getElementById("r_username").focus();
  var r_input_pw2 = document.getElementById("r_password2");
  r_input_pw2.addEventListener("keyup", function(event) {
    if (event.keyCode === 13) {
      event.preventDefault();
      document.getElementById("r_btn").click();
    }
  });
}

/**
 * Send request to server, load overview-page if request is successfull.
 */
function DoRegister() {

  //Get username, password and password verification from document.
  var json_request = new Object();
  json_request.id = document.getElementById("r_username").value;
  json_request.pw1 = document.getElementById("r_password").value;
  json_request.pw2 = document.getElementById("r_password2").value;

  //Send request
  var xhttp = new XMLHttpRequest();
  xhttp.open("POST", "/api/user_registration");
  xhttp.send(JSON.stringify(json_request));

  //Function to handle request 
  xhttp.onload = function(event){

    //If request fails, display message to user.
    if (xhttp.status == 401) {
      document.getElementById("center_box").style="border: 3px solid red;";
      document.getElementById("r_password").value="";
      document.getElementById("r_password2").value="";
      document.getElementById("login_error").style="display: block;";
      document.getElementById("login_error").innerHTML="Login failed!";
      if (this.responseText != "") 
        document.getElementById("login_error").innerHTML=this.responseText;
      SetColorAll("red");
    }
    //Load overview-page.
    else {
      window.location="/overview";
    }
  };
}

/**
 * Check, whether passwords match.
 */
function PasswordMatch() {
  if (document.getElementById("r_password").value ==
    document.getElementById("r_password2").value) {
    document.getElementById("pw_match").style = "color: green;"; 
  }
  else {
    document.getElementById("pw_match").style = "display: block";
  }
}

/**
 * function to detrmin pasword strength.
*/
function PasswordStrength() {
  document.getElementById("login_error").style="display: none;";
  let score = ScorePassword(document.getElementById("r_password").value);

  //Change color of succeded fields
  for (type of ["big", "small", "digit", "lower"]) {
    if (score[type]) 
      document.getElementById(type).style = "color: green;";
    else
      document.getElementById(type).style = "color: red;";
  }

  //Set not-used white.
  if (score.success && score.big) 
    SetColor2("white");
  else if (score.success)
    SetColor1("white");
}

/**
 * Function to determine which parts of password are correct.
 */
function ScorePassword(pass) {
  var obj = new Object;
  obj.success = false;
  obj.big = false;
  obj.small = false;
  obj.digit = false;
  obj.lower = false;

  if (!pass)
    return obj;

  if (pass.length == 15) {
    obj.big = true;
    obj.success = true;
    return obj;
  }

  if (pass.length >= 8) obj.small=true;
  if (/\d/.test(pass)) obj.digit = true;
  if (/[a-z]/.test(pass)) obj.lower = true;
  if (obj.small && obj.digit && obj.lower) obj.success = true;
  return obj;
}

function SetColor1(color) {
  document.getElementById("big").style = "color: " + color + ";";
}
function SetColor2(color) {
  for (let type of ["small", "digit", "lower"]) 
    document.getElementById(type).style = "color: " +color + ";";
}
function SetColorAll(color) {
  SetColor1(color);
  SetColor2(color);
}
