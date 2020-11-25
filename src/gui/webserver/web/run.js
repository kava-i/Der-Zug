//Close modal, when close-span is clicked (works for both moduls)
function CloseModul() {
  document.getElementById("modal_log").style.display = "none";
}

//Close modal when users clicks anywhere outside of the modal. (works for both)
window.onclick = function(event)  {
  let modal = document.getElementById("modal_log");
  if (event.target == modal) 
    modal.style.display = "none";
  else
    return;
  window.location=window.location;
}
//Function to send request, to check whether game is still running
function check_running() {
  var xhttp = new XMLHttpRequest();
  xhttp.open("POST", "/api/check_running");
  xhttp.send(window.location.pathname);
  
  xhttp.onload = function(event){
    document.getElementById("modal_log").style.display = "block";
    if (xhttp.status == 200) {
      document.getElementById("check_msg").innerHTML = "Game is running as accpected!";
    }
    else
      document.getElementById("check_msg").innerHTML = "Game is not running!";
  }
}

function get_log() {
  var xhttp = new XMLHttpRequest();
  xhttp.open("POST", "/api/get_log");
  console.log("sending request");
  xhttp.send(window.location.pathname);
  
  xhttp.onload = function(event){
    console.log("Got response: ", this.responseText);
    if (xhttp.status == 200) {
      document.getElementById("log_div").style.display="block";
      document.getElementById("display_log").innerHTML = this.responseText;
    }
    else
      document.getElementById("check_msg").innerHTML = "failed getting log.";
  }
}

function run() {
  var xhttp = new XMLHttpRequest();
  xhttp.open("POST", "/api/get_user_port");
  console.log("sending request");
  xhttp.send();
  xhttp.onload = function(event){
    var cur_loc = window.location.href;
    cur_loc = cur_loc.substr(0, cur_loc.indexOf(":", 7));
    console.log("current location:", cur_loc);
    console.log("Got port:", this.responseText);
    window.open(cur_loc + ":" + this.responseText + "/");
  }
}
