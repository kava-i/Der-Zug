function CreateEmptyTextElem(index) {
  var optional = "id='optional' style='display:none;'";
  var event_elem = "<li><input placeholder='event' value='event'></li>";
  var elem = 
    "<li id='li_description_"+index+"'>"
    + "<ul id='description'>"
    + "<li><label>Speaker:</label><input id='speaker' placeholder='speaker'></li>"
    + "<li><label>Text:</label><input id='text'></textarea></li>"
    + "<li "+optional+" class='full_length'><label>Logic:</label><input id='logic'" 
          + " placeholder='logic'></li>"
    + "<li "+optional+" class='full_length'><label>Updates:</label><input id='' "
          + " placeholder='updates' value='{}'></li>"
    + "<li "+optional+"><label style='width:auto;'>Pre peremanent Events:</label>
    + "     <ul id='pre_pEvents'>" + event_elem + "</ul></li>"
    + "<li "+optional+"><label style='width:auto;'>Pre one-time Events:</label>
    + "     <ul id='pre_otEvents'>" + event_elem + "</ul></li>"
    + "<li "+optional+"><label style='width:auto;'>Post peremanent Events:</label>
    + "     <ul id='post_pEvents'>" + event_elem + "</ul></li>"
    + "<li "+optional+"><label style='width:auto;'>Post one-time Events:</label>
    + "     <ul id='post_otEvents'>" + event_elem + "</ul></li>"
    + "</ul>"
    + "<div id='div_text_expand_"+index+' class="expand">
    + "  <span title='Delete this element.' onclick='del_text("+index+")'>-</span>"
    + "  <span id='text_expand_"+index+"' class='fas fa-caret-down' title='Delete this element.' 
          + " onclick='del_text("+index+")'>-</span>"
    + "  <span title='Add new element here.' onclick='add_text("+index+")'>-</span>"
    + "</div></li>";
  return elem;
}

