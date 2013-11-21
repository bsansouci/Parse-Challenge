function encrypt(sentence, mapping) {
  return sentence.split(' ').map(function(word){
    return word.split('').map(function(c) {
      return mapping[c.charCodeAt(0)];
    }).join('');
  }).join(' ');
}
function getRandomMapping() {
  var arr = [];
  var ASCII = [];
  for (var i = 0; i < 94; i++) {
    ASCII.push(i + 33);
  }
  for (var i = 0; i < 256; i++) {
    if(i > 32 && i < 127)
      arr[i] = String.fromCharCode(ASCII.pop(Math.random() * (ASCII.length - 1)));
    else
      arr[i] = 0;
  }
  return arr;
}
function getRandomSentence() {
  var max = 10;
  var sentence = "";
  for (var i = 0; i < max; i++) {
    var word = "";
    for (var j = 0; j < Math.random() * 40; j++) {
     word += String.fromCharCode(Math.random() * 93 + 33);
    }
    sentence += word + "\n";
  }
  sentence = sentence.slice(0, sentence.length - 1);
  return sentence;
}

var s = getRandomSentence();
var a = getRandomMapping();
var r = encrypt(s, a);
console.log("BEFORE: " + s + "\n\nAFTER: " + r);
