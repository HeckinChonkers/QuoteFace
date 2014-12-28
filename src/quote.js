// Listen for when the watchface is opened
Pebble.addEventListener('ready', 
  function(e) {
    console.log("PebbleKit JS ready!");
    
    //Get the initial quote
    getQuote();
  }
);

// Listen for when an AppMessage is received
Pebble.addEventListener('appmessage',
  function(e) {
    console.log("AppMessage received!");
    getQuote();
  }                     
);

function getQuote(){
    var quoteRegEx = /quotation" href="\/quote\/[0-9]*.html">[A-Za-z0-9'. ,&";?%]*<\/a>/g;
    var authorRegEx = /<b><a href="\/quotes\/[A-Za-z0-9_ .]*\/">[A-Za-z0-9 .]*<\/a>/g;
    var request = new XMLHttpRequest();
    request.open('GET', 'http://www.quotationspage.com/random.php3', false);
    request.send();
    if(request.status==200){
      var i = 0;
      var editquote,editauthor;
      while(i < 20){
        var quote = quoteRegEx.exec(request.responseText);
        console.log(quote[0]);
        var author = authorRegEx.exec(request.responseText);
        console.log(author[0]);
      
        var begin = quote[0].indexOf(">") + 1;
        var end = quote[0].lastIndexOf("<");
        editquote = quote[0].substring(begin, end);
        console.log("Quote is " + editquote);
      
        begin = author[0].indexOf("/\">") + 3;
        end = author[0].lastIndexOf("<");
        editauthor = author[0].substring(begin, end);
        console.log("By " + editauthor);
        
        if(editquote.length > 120 || editauthor.length > 20)
          i++;
        else
          i = 20;
      }
      //Assemble dictionary
      var dictionary = {
        "KEY_QUOTE": editquote,
        "KEY_AUTHOR": editauthor
      };
      
      // Send to Pebble
      Pebble.sendAppMessage(dictionary,
             function(e) {
              console.log("Quote sent to Pebble successfully!");
             },
             function(e) {
               console.log("Error sending quote to Pebble!");
             }
      );
      
    }else{
      console.log("Error getting website!");
    }
}