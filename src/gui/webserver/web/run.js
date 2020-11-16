//Function to send request, to check whether game is still running
function check_running() {

  var xhttp = new XMLHttpRequest();
  xhttp.open("POST", "/api/check_running);
  xhttp.send(window.location.pathname);
  
  xhttp.onload = function(event){
    if (xhttp.status == 200) {
      alert("Game is running as accpected!");
    else
      alert("Game is not running");
    }
}
