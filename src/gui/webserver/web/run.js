//Function to send request, to check whether game is still running
function check_running() {
  var xhttp = new XMLHttpRequest();
  xhttp.open("POST", "/api/check_running");
  xhttp.send(window.location.pathname);
  
  xhttp.onload = function(event){
    document.getElementById("modal_log").style.display = "block";
    document.getElementById("check_msg").style.display = "block";
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
  xhttp.send(window.location.pathname);
  
  xhttp.onload = function(event){
    console.log("text: ", this.responseText);
    if (xhttp.status == 200) 
      document.getElementById("display_log").innerHTML = this.responseText;
    else
      document.getElementById("display_log").innerHTML = "failed getting log.";
  }
}
