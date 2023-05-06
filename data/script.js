
//*******************************************/
//Fonction pour afficher les drink possibles
//*******************************************/



        
// Fonction pour générer les sections HTML pour chaque breuvage
function genererBreuvages() {
    // Récupérer l'élément de liste
    //var liste = document.getElementById("breuvages");
    var liste = document.getElementById("listeDrinksPossible");
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



var V_Bouteille1 = "";
var V_Bouteille2 = "";
var V_Bouteille3 = "";
var V_Bouteille4 = "";
var V_Bouteille5 = "";
var V_Bouteille6 = "";
var V_Bouteille7 ="";
var V_Bouteille8 ="";
var V_Bouteille9 ="";
var V_Bouteille10 ="";


$(document).ready(function(){             // cette fonction attend que le bouton APPLIQUER sois appuyer et envoie la valeur au ESP32
  $("#appliquer").click(function(){
    V_Bouteille1 = $("#Bouteille1").val();
    V_Bouteille2 = $("#Bouteille2").val();
    V_Bouteille3 = $("#Bouteille3").val();
    V_Bouteille4 = $("#Bouteille4").val();
    V_Bouteille5 = $("#Bouteille5").val();
    V_Bouteille6 = $("#Bouteille6").val();
    V_Bouteille7 = $("#Bouteille7").val();
    V_Bouteille8 = $("#Bouteille8").val();
    V_Bouteille9 = $("#Bouteille9").val();
    V_Bouteille10 = $("#Bouteille10").val();
      $.post("calibrationPompes",{
          BouteilleNo1: V_Bouteille1,
          BouteilleNo2: V_Bouteille2,
          BouteilleNo3: V_Bouteille3,
          BouteilleNo4: V_Bouteille4,
          BouteilleNo5: V_Bouteille5,
          BouteilleNo6: V_Bouteille6,
          BouteilleNo7: V_Bouteille7,
          BouteilleNo8: V_Bouteille8,
          BouteilleNo9: V_Bouteille9,
          BouteilleNo10: V_Bouteille10
      });
      alert("LES BOUTEILLES ONT ÉTÉ ENREGISTRÉES");
  });
});

//*************Quand le breuvage a été choisi envoi l'info au ESP32*************************/
$(document).ready(function(){             // cette fonction attend que le bouton  sois appuyer et envoie la valeur au ESP32
  $("#FaireBreuvage").click(function(){
      var drink = $("#drinkPossibles").val();
      $.post("faireDrink",{
        drink_voulue: drink 
      });
      alert("LE BREUVAGE EST EN COURS DE PREPARATION");
  });
});

function resetSelections() {
  $.post("calibrationPompes",{
    BouteilleNo1: "Bouteille no1",
    BouteilleNo2: "Bouteille no2",
    BouteilleNo3: "Bouteille no3",
    BouteilleNo4: "Bouteille no4",
    BouteilleNo5: "Bouteille no5",
    BouteilleNo6: "Bouteille no6",
    BouteilleNo7: "Bouteille no7",
    BouteilleNo8: "Bouteille no8",
    BouteilleNo9: "Bouteille no9",
    BouteilleNo10: "Bouteille no10"
  });

  // Rafraîchir la page
  location.reload();
  
}


document.addEventListener("DOMContentLoaded", function() {
    getB1();
    getB2();
    getB3();
    getB4();
    getB5();
    getB6();
    getB7();
    getB8();
    getB9();
    getB10();
});



function getB1()
{
    var xhttp = new XMLHttpRequest();
    xhttp.onreadystatechange = function()
    {
        if(this.readyState == 4 && this.status == 200)
        {
            document.getElementById("b1").innerHTML = this.responseText;  
        }
    };
    xhttp.open("GET", "afficheBouteille_1_Precedentes", true);
    xhttp.send();
}

function getB2()
{
    var xhttp = new XMLHttpRequest();
    xhttp.onreadystatechange = function()
    {
        if(this.readyState == 4 && this.status == 200)
        {
            document.getElementById("b2").innerHTML = this.responseText;  
        }
    };
    xhttp.open("GET", "afficheBouteille_2_Precedentes", true);
    xhttp.send();
}

function getB3()
{
    var xhttp = new XMLHttpRequest();
    xhttp.onreadystatechange = function()
    {
        if(this.readyState == 4 && this.status == 200)
        {
            document.getElementById("b3").innerHTML = this.responseText;  
        }
    };
    xhttp.open("GET", "afficheBouteille_3_Precedentes", true);
    xhttp.send();
}

function getB4()
{
    var xhttp = new XMLHttpRequest();
    xhttp.onreadystatechange = function()
    {
        if(this.readyState == 4 && this.status == 200)
        {
            document.getElementById("b4").innerHTML = this.responseText;  
        }
    };
    xhttp.open("GET", "afficheBouteille_4_Precedentes", true);
    xhttp.send();
}

function getB5()
{
    var xhttp = new XMLHttpRequest();
    xhttp.onreadystatechange = function()
    {
        if(this.readyState == 4 && this.status == 200)
        {
            document.getElementById("b5").innerHTML = this.responseText;  
        }
    };
    xhttp.open("GET", "afficheBouteille_5_Precedentes", true);
    xhttp.send();
}

function getB6()
{
    var xhttp = new XMLHttpRequest();
    xhttp.onreadystatechange = function()
    {
        if(this.readyState == 4 && this.status == 200)
        {
            document.getElementById("b6").innerHTML = this.responseText;  
        }
    };
    xhttp.open("GET", "afficheBouteille_6_Precedentes", true);
    xhttp.send();
}

function getB7()
{
    var xhttp = new XMLHttpRequest();
    xhttp.onreadystatechange = function()
    {
        if(this.readyState == 4 && this.status == 200)
        {
            document.getElementById("b7").innerHTML = this.responseText;  
        }
    };
    xhttp.open("GET", "afficheBouteille_7_Precedentes", true);
    xhttp.send();
}

function getB8()
{
    var xhttp = new XMLHttpRequest();
    xhttp.onreadystatechange = function()
    {
        if(this.readyState == 4 && this.status == 200)
        {
            document.getElementById("b8").innerHTML = this.responseText;  
        }
    };
    xhttp.open("GET", "afficheBouteille_8_Precedentes", true);
    xhttp.send();
}

function getB9()
{
    var xhttp = new XMLHttpRequest();
    xhttp.onreadystatechange = function()
    {
        if(this.readyState == 4 && this.status == 200)
        { 
            document.getElementById("b9").innerHTML = this.responseText;  
        }
    };
    xhttp.open("GET", "afficheBouteille_9_Precedentes", true);
    xhttp.send();
}

function getB10()
{
    var xhttp = new XMLHttpRequest();
    xhttp.onreadystatechange = function()
    {
        if(this.readyState == 4 && this.status == 200)
        { 
            document.getElementById("b10").innerHTML = this.responseText;  
        }
    };
    xhttp.open("GET", "afficheBouteille_10_Precedentes", true);
    xhttp.send();
}








// // Fonction pour enregistrer les sélections dans localStorage
// function saveSelections() {
//   var selections = {
//     Bouteille1: V_Bouteille1,
//     Bouteille2: V_Bouteille2,
//     Bouteille3: V_Bouteille3,
//     Bouteille4: V_Bouteille4,
//     Bouteille5: V_Bouteille5,
//     Bouteille6: V_Bouteille6,
//     Bouteille7: V_Bouteille7,
//     Bouteille8: V_Bouteille8,
//     Bouteille9: V_Bouteille9,
//     Bouteille10: V_Bouteille10   
//   };
//   localStorage.setItem("selections", JSON.stringify(selections));
// }

// // Fonction pour charger les sélections depuis localStorage
// var savedSelections = JSON.parse(localStorage.getItem("selections")) || {};
// V_Bouteille1 = savedSelections.Bouteille1 || "";
// V_Bouteille2 = savedSelections.Bouteille2 || "";
// V_Bouteille3 = savedSelections.Bouteille3 || "";
// V_Bouteille4 = savedSelections.Bouteille4 || "";
// V_Bouteille5 = savedSelections.Bouteille5 || "";
// V_Bouteille6 = savedSelections.Bouteille6 || "";
// V_Bouteille7 = savedSelections.Bouteille7 || "";
// V_Bouteille8 = savedSelections.Bouteille8 || "";
// V_Bouteille9 = savedSelections.Bouteille9 || "";
// V_Bouteille10 = savedSelections.Bouteille10 || "";

// // affichage des sélections précédentes dans les selects correspondants
// document.getElementById("Bouteille1").value = V_Bouteille1;
// document.getElementById("Bouteille2").value = V_Bouteille2;
// document.getElementById("Bouteille3").value = V_Bouteille3;
// document.getElementById("Bouteille4").value = V_Bouteille4;
// document.getElementById("Bouteille5").value = V_Bouteille5;
// document.getElementById("Bouteille6").value = V_Bouteille6;
// document.getElementById("Bouteille7").value = V_Bouteille7;
// document.getElementById("Bouteille8").value = V_Bouteille8;
// document.getElementById("Bouteille9").value = V_Bouteille9;
// document.getElementById("Bouteille10").value = V_Bouteille10;


// // Écouteur d'événements pour enregistrer les sélections avant de quitter la page
// window.addEventListener("beforeunload", function() {
//   saveSelections();
// });


// function resetSelections() {
//   localStorage.clear();
  
//   V_Bouteille1 = "";
//   V_Bouteille2 = "";
//   V_Bouteille3 = "";
//   V_Bouteille4 = "";
//   V_Bouteille5 = "";
//   V_Bouteille6 = "";
//   V_Bouteille7 = "";
//   V_Bouteille8 = "";
//   V_Bouteille9 = "";
//   V_Bouteille10 = "";

//   // Rafraîchir la page
//   location.reload();
  
// }







