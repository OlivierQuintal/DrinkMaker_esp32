

// Tableau de breuvages
var breuvages = ["Café", "Thé", "Jus d'orange", "Limonade"];
        
// Fonction pour générer les sections HTML pour chaque breuvage
function genererBreuvages() {
    // Récupérer l'élément de liste
    var liste = document.getElementById("breuvages");

    // Vider la liste
    liste.innerHTML = "";

    // Boucler sur les breuvages et générer une section pour chaque breuvage
    for (var i = 0; i < breuvages.length; i++) {
        var breuvage = breuvages[i];
        var section = document.createElement("section");
        section.innerHTML = "<h2>" + breuvage + "</h2>";
        liste.appendChild(section);
    }
}