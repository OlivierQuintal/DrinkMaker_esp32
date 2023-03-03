

//var drink = ["Baileys Irish Cream", "Bourbon", "club soda", "Gin", "Ginger ale", "lemonade", "lime", "Malibu coconut rum", "milk", "Orange Juice", "peach schnapps", "Rhum noir", "Rhum blanc", "Sprite", "Tequila", "Tonic water", "Triple Sec", "Vodka"];

function choixBouteille()
{
  document.getElementById("inventaire").classList.toggle("show");
}

//*******************************************/
//Fonction pour afficher les drink possibles
//*******************************************/
// Tableau de breuvages
var breuvages = ["Café", "Thé", "Jus d'orange", "Limonade"];
        
// Fonction pour générer les sections HTML pour chaque breuvage
function genererBreuvages() {
    // Récupérer l'élément de liste
    var liste = document.getElementById("breuvages");

    // Vider la liste
    //liste.innerHTML = "";
    liste.innerHTML = "<option value=100>Choisir un breuvage</option>";

    // Boucler sur les breuvages et générer une section pour chaque breuvage
    for (var i = 0; i < breuvages.length; i++) {
        var breuvage = breuvages[i];
        //var section = document.createElement("section");
        var section = document.createElement("option");
        //section.innerHTML = "<button>" + breuvage + "</button>";
        section.innerHTML = "<option value="+i+">" + breuvage + "</option>";
        liste.appendChild(section);
    }
}
/*************************************************/

$(document).ready(function(){             // cette fonction attend que le bouton APPLIQUER sois appuyer et envoie la valeur au ESP32
  $("#appliquer").click(function(){
      var Bouteille1 = $("#Bouteille1").val();
      var Bouteille2 = $("#Bouteille2").val();
      var Bouteille3 = $("#Bouteille3").val();
      var Bouteille4 = $("#Bouteille4").val();
      var Bouteille5 = $("#Bouteille5").val();
      var Bouteille6 = $("#Bouteille6").val();
      var Bouteille7 = $("#Bouteille7").val();
      var Bouteille8 = $("#Bouteille8").val();
      var Bouteille9 = $("#Bouteille9").val();
      var Bouteille10 = $("#Bouteille10").val();
      $.post("calibrationPompes",{
          BouteilleNo1: Bouteille1,
          BouteilleNo2: Bouteille2,
          BouteilleNo3: Bouteille3,
          BouteilleNo4: Bouteille4,
          BouteilleNo5: Bouteille5,
          BouteilleNo6: Bouteille6,
          BouteilleNo7: Bouteille7,
          BouteilleNo8: Bouteille8,
          BouteilleNo9: Bouteille9,
          BouteilleNo10: Bouteille10
      });
  });
});

//*************Quand le breuvage a été choisi envoi l'info au ESP32*************************/
$(document).ready(function(){             // cette fonction attend que le bouton APPLIQUER sois appuyer et envoie la valeur au ESP32
  $("#FaireBreuvage").click(function(){
      var drink = $("#drinkPossibles").val();
      $.post("faireDrink",{
        drink_voulue: drink 
      });
  });
});


// getBreuvages(function getdata()
// {
//     var xhhtp = new XMLHttpRequest();

//     xhhtp.onreadystatechange = function()
//     {
//         if (this.readyState == 4 && this.status == 200)
//         {
//           document.getElementById("listeBreuvages").innerHTML = this.responseText;
//         }
//     };
//     xhhtp.open("GET", "tableauBreuvages", true);
//     xhhtp.send();
// },2000);

function chargerBreuvages()
{
    var xhhtp = new XMLHttpRequest();

    xhhtp.onreadystatechange = function()
    {
        if (this.readyState == 4 && this.status == 200)
        {
          document.getElementById("listeBreuvages").innerHTML = this.responseText;
        }
    };
    xhhtp.open("GET", "tableauBreuvages", true);
    xhhtp.send();
}



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



