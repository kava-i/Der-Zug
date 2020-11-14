/**
 * Open modal to delete user. 
 * Modal asks user, whether he/she really wants to delete account.
 */
function OpenDelUserModal() {
  document.getElementById("modal_del_user").style.display = "block";
}
//Close modal, when close-span is clicked (works for both moduls)
function CloseModul() {
  document.getElementById("modal_del_user").style.display = "none";
  window.location=window.location;
}

//Close modal when users clicks anywhere outside of the modal. (works for both)
window.onclick = function(event)  {
  let modal = document.getElementById("modal_del_user");
  if (event.target == modal) 
    modal.style.display = "none";
  else
    return;
  window.location=window.location;
}

/**
 * function to logout user.
 */
function DoLogout() {
  
  //Send request
  console.log("sending logout-request!");
  var xhttp = new XMLHttpRequest();
  xhttp.open("POST", "/api/user_logout");
  xhttp.send();

  //Reload, which redirects to main-page.
  xhttp.onload = function(event){
      window.location="/";
    };
}


/**
 * function deleting deleteing.
 */
function DelUser() {
  //Send request
  var xhttp = new XMLHttpRequest();
  xhttp.open("POST", "/api/user_delete");
  xhttp.send("");

  //Function to handle request 
  xhttp.onload = function(event){
    //If request fails, display message to user.
    let msg = document.getElementsByClassName("user_error")[0];
    if (xhttp.status == 401) {
      msg.style = "display: block;"; 
      msg.innerHTML = "Could not be deleted. Sorry for that, try again later.";
    }
    //Display success message to user.
    else {
      msg.style= "display: block; color: green;"; 
      msg.innerHTML = "Successfully deleted account.";
      document.getElementById("btn_del_user").style="display: none;";
      window.location="/";
    }
  }
}
