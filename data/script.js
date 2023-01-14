

var drink = ["Baileys Irish Cream", "Bourbon", "club soda", "Gin", "Ginger ale", "lemonade", "lime", "Malibu coconut rum", "milk", "Orange Juice", "peach schnapps", "Rhum noir", "Rhum blanc", "Sprite", "Tequila", "Tonic water", "Triple Sec", "Vodka"];

function choixBouteille()
{
  document.getElementById("inventaire").classList.toggle("show");
}


$(document).ready(function(){             // cette fonction attend que le bouton APPLIQUER sois appuyer et envoie la valeur au ESP32
  $("#appliquer").click(function(){
      var pompe1 = $("#Pompe1").val();
      var pompe2 = $("#Pompe2").val();
      var pompe3 = $("#Pompe3").val();
      var pompe4 = $("#Pompe4").val();
      var pompe5 = $("#Pompe5").val();
      var pompe6 = $("#Pompe6").val();
      var pompe7 = $("#Pompe7").val();
      var pompe8 = $("#Pompe8").val();
      $.post("calibrationPompes",{
          PompeNo1: pompe1,
          PompeNo2: pompe2,
          PompeNo3: pompe3,
          PompeNo4: pompe4,
          PompeNo5: pompe5,
          PompeNo6: pompe6,
          PompeNo7: pompe7,
          PompeNo8: pompe8
      });
  });
});


function filterFunction()
{
  var input, filter, ul, li, a, i;
  input = document.getElementById("myInput");
  filter = input.value.toUpperCase();
  div = document.getElementById("inventaire");
  a = div.getElementsByTagName("a");
  for (i = 0; i < a.length; i++) {
    txtValue = a[i].textContent || a[i].innerText;
    if (txtValue.toUpperCase().indexOf(filter) > -1) {
      a[i].style.display = "";
    } else {
      a[i].style.display = "none";
    }
  }
}



