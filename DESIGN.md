**Under construction**


# Design

This is a design document, mainly for me, but if you're interested please feel free. It'll talk about design pillars and general best practices so that it's clear to both me and possible future team members what the game we're trying to make actually is.

## Scope

This is a CRPG for Windows/Linux/Mac which takes between **2 to 4 hours** to complete. Something between a smaller indie CRPG and a verticle slice/tech demo. The core goal is to showcase each system at their fullest, perhaps not at their best, but to demonstrate what engine can do, how, and give a feel of a full game could look like. 

## Goals

There are a couple of things which Plamol is not.

It isn't:
- A sprawling sandbox open world.
- A combat focused TRPG.
- A linear ARPG.
- A RPG without consequences.

The goal is depth over scale. If the game takes 20 minutes to complete, but is totally different depending on playstyle/build, then thats a success. 

This means, no trashy encounters; we want combat to be meaningful and in service of demonstarting what the game is. 
No linear and or uninteresting quest; we want to player to have control over their approach, and to not be punished for specific builds or character traits.   
And we want build and character variety without sacificing playablity; a quest should have at least three solutions: A stealthy approach - a violent approach - and a talk approach.
And most importantly, we want scalability and clear quest structure. Quest should funnel into choke points when they can, and when quest diverge based on player choice, they shouldn't be hollow.

## Core Loop

The core loop of Plamol in simplest terms is: find - explore - do - resolve - evolve 

- Find: Find means naturally comming upon something of interest. We want the player to feel a sense of freedom and personal direction. A great example of the Find principle is found in Fallout 1; where the player is presented with Vault 15 on their map, and **find** Shady Sands, in between their starting location and Vault 15. This is how we want to introduce quests, locations, items, and NPCs to players. And we want the player to have a reason to stop by, but not necessarily be blocked by it. For Fallout 1, this comes in the form of the rope item, which is required to decend into Vault 15, and can be found in Shady Sands. Importantly none of these are required to complete the game. You can go the final area of the game, the church (although this is due to a bug) and complete the game without ever touching a rope.

- Explore: Explores means presenting the player with an enviorment, and trusting their deductive reasons and natural drive for exploration. A bad example of explore would be a quest marker pointing to a merchant who can sell you the item you need. Where as a good example would be presenting the player with an area where the required item can be found in multiple places through exploration. The best thing is explore can lead into a branch of the core loop and act as the entrance to a new Find node.

- Do: Do means allowing the player to act. As developers we should present the player with tools of success, while stepping back and letting players experience the world through their own eyes, instead of through our ordaned paths. Do is exemplified by the first quest of Fallout 1 through the action of using the rope on the elevator. We've presented the player with tools to success (Bartering/Combat/Skills/Dialogue/Travel/Descriptions) and allowed them to act on their own intution and will. You can find the rope through a small shack on the back of Shady Sands, you can purchase one off the front guard, or initate combat or pickpocket it off the guard. We want to reward players for inuitive thinking, and discourdge railroading.

- Resolve: Resolve can mean two things. Either a choke point, or a divergence. We want to give the player the support and freedom to solve problems themselves, but often we're unable to always diverge the plot for everything, which is why we use choke points. To continue the Fallout 1 example, the rope quest always resolves at the same point, you use rope to descend down the elevator and find your path to the command room blocked by rubble. This is a choke point, how you arrive here is not contingent on the resolution of the quest. As for divergence we can look to another Fallout game, Fallout New Vegas. The divergence seen in Fallout New Vegas is the choice between alling oneself with the core factions of the game: The NCR, The Legion, Mr House, or Independance. This is an example of what a divergence should be. A divergence should meaningfully alter the plot, and genuinely change how the game is played from then on. If a divergence cannot meet those requirments, then it shouldn't be a divergence.

- Evolve: Lastly is evolve. Evolve revolves around a personal theory about CRPGs. So this will be the longest section. 

## Evolve

When the player boots the game, and the start screen shows up, and they are met with an empty character creation screen, there is a fundemental metaphysical contradiction on display. The player is incapable of doing anything better than anything else, but it is also truly incapable of failing at anything worse than anything else. So we should ask what is this character? The character is limited by it's reality, in this case, the rpg system at play. The character is incapable of exceeding the bounds of the system. So, we can say then, that it is defined by it's reality. So when we see it, void of human intervention, we are not seeing a character, a subject of a conscious entity; but we are seeing a total manifestation of reality. It is both indicitive of the limits of reality, and meaningless as a subject of reality. So the contradiction is established, this object is at once part and equal to the whole. 

We can lay this out mathmatically. If M is total number of possible combination of skills and stat choices, then we can examine the object as N, the number of possiblities available to the subject as of now. What is N here specifically, N tracks not only the the mechanical restraints of the system (I cannot wield an Axe because it requires 5 strength and I have 4) which limit the possiblities of builds, but also the possible combinations available to the player themself. Let us take two players, one has never played the game before, and the other has played it a thousand times. They each say "I'm going to make a cowboy character.". At this moment, player one who has never played the game before, is at N=M. The builds available to him are everything, because he lacks the understanding of the system. Wether he chooses the fast shot perk, or the cowboy perk first, is not dependant on the him saying "I'm going to make a cowboy" before he even launched the game. Let us now look at player two. Player two is N=1, player two knows what he will do, why, when, and how. The possible character combinations for Player two are exactly one, there is no other character Player two will play other than this specific formualtion of cowboy.   

This is the contradiction of the Computer Role Playing Game. What it means to beat a CRPG, is to resolve the contradiction. And importantly, when N goes down, it is a sublation, not a reduction. The act of choosing is meaningful relative to the ontology of your character. Through the choices made by the player, they are creating the limits which future decisions will have to sublate. Let us the example of the cowboy again. The cowboy is presented the opportunity to give up knifes, permently. The choice is made, and he will reckon with those consequences. An hours later the cowboy is severly injuried by an Alien, because it dodge the bullets. His commitment to guns created that limit, so when he chooses to invest in stealth to catch the Alien by surprise, he is directly answering to that limit. He has evolved into something which encompases not only what he is, but what he isn't. Cowboy now means something entierly different and relative to him not being able to use knifes. Him not being able to use knifes is as important to his character as if he was able to.       

But the difficulty is this narrowing does not progress linearly, it gets exponentially more and more difficult to narrow down the possible builds for the developer. The decay in cut size grows enormously as you eliminate possible builds, so the end of the game truly feels like your character is changing. Like they are at the climax; met with the belly of the beast or obi wans voice as you stair down the Death Start trenches. Interely because the options presented to you are expressly yours, the character you've built has been built around these options.  

This is a side note. This idea is Hegelian dialectics, there so much overlap between the two. Either CRPGs are describing the phenomology of consciousness, or Hegel was just really ahead of his time. This is absolutly not my work, I've done my best to describe one of the most complicated ideas in western philosophy, and at the same time try to demonstrate how applicable it is to CRPGs as a way of describing why we love them so much. So I do hope this at least partially demonstrates why I feel CRPGs are the way the are as pieces of art. I think inherently CRPGs are demonstrating a very particular idea about consciousness and human evolution which if we understood we could harness to make a CRPG which hits all the marks.                       
## Tone 

We want the player to feel a sense of isolation. They are a person in limbo, in capable of finding home, they are in state of contradiction which can only be resolved at the end of the game.

But this doesn't mean the player shouldn't feel joy and hope, but that the player must reckognize that these places they walk through, however joyful they may or may not be, is not home.

The best resource I can point to is the bronze age Swamp Thing comics. The Swamp Thing is in search for an idea beyond material reality, he is so isolated and deeply ostrized. He finds terrible terrible things, but there is joy, hope, love. He can reckognize that perhaps this may not be home, but that also the fruits of life are not so far from his grasp.  

Lastly, very importantly. The player should feel a sense that this is their adventure. That they aren't following a linear story with combat, but making decisions which matter and influence how the game plays.
## Choice

This is very much a written down interpretation of Josh Sawyers 2012 talk at GDC: Choice Architecture, Player Expression, and Narrative Design in Fallout: New Vegas. Please keep that in mind that this isn't my creation.

Good choice, specifically speech, should rely on extra deductive information. Passing a skill check should make the options to succeed in a certain way available, instead of just succeeding. Same should be said for non-speech based characters, this means notes, hearsay, rumors, which can give the player the advantage in the conversation where speech doesn't. Perhaps the player makes a logical argument against the NPCs position, and they are punished for it because they didn't anticipate the NPC didn't have the intelligence to understand the argument. Perhaps they are too intelligent and the a battle of brains happens. The important thing is evening punishment, a strenght based player shouldn't be punished just because they wanted to play a strength guy.
   
### Writing

This is a short guide on writing dialogue. A concept I want to introduce is Player Archtypes. Archytypes are generalized characters which are used for dialogue response. Noteably these are independant of theming or specifics, but are more so a form of presenting a emotional reaction. For example you can have an archtype which is rude, or doesn't like talking, or someone whos incredibly kind and generouse. These should be decided as global archytypes before writing any dialogue, most if not all responses should have these archtypes. This creates a clearer sense of tone, and a clearer style to the responses.

The flow for writings goes like this

- Define what the player needs to know by the end of the dialogue.
- Define what the player currently knows.
- Define the global range of character expression.
- Create paths from what the player knows to what the player needs to know for each form of expression.
- Define how each path challenges the player.
- Define and forecast how each path changes the world based on the kind of player being shown.
- Ensure the challenge and change are not uneven in their severity.
- Validate all choices.
- Ensure the information the player needs to know is obtainable without inisitating dialogue.
- Consider if this is a diverging choice which branches the story.
- Evaluate if the choices are intresting and satisfying.
- Write prose.

## Design Pillars

Design pillars are intrumental to clear design, especially in teams. So I will try to be clear and upfront with these. For future team members, if you read anything, this is what you read. These pillars of non-negotiable and reflect the game that we are making. A failure to understand these means you will be developing an entierly different game from everyone else. A cohesive game experience relys on these pillars being considered by all team members. 

- 1: No right solution:
    The problems we present to players should always be challenging exersices in moral judgement. Having interesting questions and meaningful decisions to choose from, make CRPGs what they are. A CRPG with shallow or uninteresting ideas and choices can come off as cheap and low effort TRPGs. We want the player to be challenged, not just mechanically, but challenged on their philosophical positions as people. This means when we present real political questions, we treat it with care and respect. We present the options based on real philosophical positions held by people, with care, attention and respect. But importantly, we only present options which are meaningful. A homophobic or rascist option is not a meaningful philosophical position to hold, we can be respectful and clear about positions we don't agree with, without presenting irrational and or false positions.
- 2: Trust the player:
   Trusting the player means respecting their capacity for inuitive thinking and challenge. This is meaningful because as a player, being rewarded for inutition and persistence creates meaningful interactions with the game. Shallow design which front faces solutions and spotlights progression create an experience where the player see their role in their story as nothing but an arbitrary accessory to the descisions designers made. Trusting the player and giving them space and time to learn and experiment for themselves creates a more interesting and captivating experience, where players feel they have agency and control over their own actions.
- 3: Interactivity:      
    From Rogue to Darklands to Nox's physics interactions to the freedom of enviorment in Deus Ex, we can learn that agency and trust comes not only from quest design and build variety, but from the tools we give the player. Something as simple as being able to use your skills on anything in Fallout 1 meaningfully alter the impression a game gives the player. Thats why interactivity is a pillar, trust and agency are meaningless when the player is unequipped to act on intution and enviorment with freedom. Not only should we eqiupe the player with the tools of interactivity, we should reward curious players for their engagement.
- 4: Reactivity:
    Arguably reactivity is the final ingredient in player agency. End slides and divergences in plot give the players actions meaning beyond the experience and loot they gain. It communicates their actions matter and are listened to by the game. The consequences of the players actions are a signel that the game can react and listen to who the player wants to play.Thats why reactivity is so important, and why it's a pillar.
- 5: Non-Linearity:
    Non-Linearity means the game can anticipate, react, and support players who choose not to do content. It's important to the sense of adventure and player trust. This means no essential NPCs or quests. The game should be beatable within 20 minutes. 
- 6: One core goal across the entire game:
    There is a single goal always present in the game, something you are always moving towards from the beginning to the end. The actual goal shouldn't change, but what it means, how it happens, and who it happesn too, should. This means the player is never left without purpose, and that 
- 7: No companions:  
- 8: No controlling the player:
    NO RANDOM ENCOUNTERS
- 9: Detailed system with endless viable possiblities. If you can tink it you can make it. And complicated enough where metagamers can get really powerful
