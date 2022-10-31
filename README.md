# Compilation et lancement
Afin de lancer le programme, executer dans l'ordre les commandes suivantes:
1. Executer la commande : ```make```
2. Puis lancer le server avec : ```./bin/server```
3. Puis lancer le client avec : ```./bin/client 127.0.0.1```

# Côté client
Une fois que la connexion est établie avec le serveur, le client aura la possibilité de s'authentifier en se connectant à son compte ou en créant un nouveau compte. <br /> <br />
Pour créer un nouveau compte : 
```sh
/register <username> <password>
```
<br />
Pour se connecter à son compte : 

```sh
/login <username> <password>
```
<br />


## Liste des fonctionnalités implémentées
<br>

| Fonctionnalités | <div style="width:230px">Commandes</div> | Détails |
| --------------- | ----------- |--------- |
|Envoi d'un message public | ```/public <message> ```| Un message est envoyé à tous les utilisateurs connectés  |
|Envoi d'un message privé | ```/private <username> <message>``` | Un message est envoyé à un seul utilisateur. Si le destinataire est déconnecté au moment de l'envoi, celui ne recevra le message qu'une fois reconnecté | 
| Envoi d'un message dans un groupe | ```/group <groupname> <message>``` |Un message est envoyé à tous les membres du groupe connectés au moment de l'envoie | 
| Creation d'un groupe | ```/create <groupname>``` |Creation d'un nouveau groupe | 
| Invitation à rejoindre un groupe | ```/invite <groupname> <username>``` |Toute personne membre d'un groupe, pourra inviter un autre utilisateur à rejoindre le groupe | 
| Rejoindre un groupe | ```/join <groupname>``` |Un utilisateur pourra rejoindre un groupe selement si ce dernier y a été invité | 
<hr>
<br>
La commande ci-dessous permet de lister toutes les commandes possibles : 

```sh
/help
```
<br />

# Protocole de communication

Afin de réaliser la communication entre le serveur et le client, nous avions procédé de la manière suivante :<br>

- Le client peut envoyé une requête au serveur via l'objet **Request** : 
```c
typedef struct{
  request_type type;                        // Le type de la requête
  int paramCount;                           // Le nombre de paramètres envoyés
  char params[PARAM_NUMBER][PARAM_SIZE];    // Les paramètres de la requête
  Message message;                          // Le message à envoyer si nécessaire
} Request;
```

- Le serveur répondra au client avec un objet **Response** :
```c
typedef struct{
  response_type type;                       // Le type de la réponse
  int paramCount;                           // Le nombre de paramètres envoyés
  char params[PARAM_NUMBER][PARAM_SIZE];    // Les paramètres de la réponse
  Message message;                          // Le message à envoyer si nécessaire
} Response;
```

