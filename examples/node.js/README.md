#\#SERVER

I Kunstig Intelligens skal du lage nettopp det - en kunstig intelligens (AI). Her har du et 
eksempel på hvordan du kan lage en litt mindre intelligent AI som du kan bruke akkurat som du vil.

# Filer

`server.js` &ndash; Håndterer tilkobling og kommunikasjon. Dette er filen som sender informasjon fra
serveren videre nedover i systemet.

`gamestate.js` &ndash; Denne filen håndterer selve gamestaten. Den vet om alt som skjer på kartet,
og skal regne ut hva som vil være en fornuftig respons. Men den trenger hjelp av både kartleser og
navigatør for å finne ut hvordan det skal gjøres.

`parser.js` &ndash; Oversetting av kartet fra server til noe vi kan bruke. Kartet blir oversatt til
et 2 dimensjonalt array hvor hvert punkt er en vekt for hvor lurt det er å gå et sted. Når du
implementerer din algoritme for å regne ut stier er det lurt å bruke disse vektene til å finne ut hvor
det er attraktivt å gå.

`navigation.js` &ndash; Dette er navigatøren din. Navigatøren vil ta i mot en beskjed om hvor vi skal,
og deretter finne ut hvordan vi kommer oss dit. Ved bruk pathfinding algoritmen din vil navigatøren
se på hvor du vil, og skrike ut om det er HØYRE, VENSTRE, FRAMOVER eller BAKOVER. Denne beskjeden 
sender du videre til til serveren fra gamestate.

`start.bat` &ndash; Brukes til å starte AIen under selve konkurransen.

Lykke til!
