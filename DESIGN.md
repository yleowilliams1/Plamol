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

- Evolve: Lastly is evolve. The crux of story is the evolution and change of character, so in many ways evolve is the most important out of all. We evolve the player in two ways, mechanically, and ontologically. We can mechanically evolve the player through new enemies, areas, and mechanics. Continuing the Fallout example, this can be seen in the Perks system, where the player chooses bonuses to their character. The Fallout Perks system achieves this evolution through two distinct factors. The first is the illusion of character growth through mechanics. A +5 to guns meaningfully demonstrates a character position, giving the illusion of mechanical character growth. But mechanical growth by itself is not enough, these a ludonarrative dissonance between the state of the player character, and the player. So we much ontologically evolve the character beyond mechanics. This is done through altering the players perception of their character through dialetical contradition. If we imagine the player character as a subject in contradition, evolution resolves the contradiction. If the game has M possible character builds, and the number of character builds available to the player at any time is N; then at character creation N = M. The goal of ontological change is to narrow N as far as possible. Ideally at the end of the game N should equal 1, that is the player is only capable of being one kind of build, itself. This is done through resolving the internal contradiction of a character. At character creation the character is in direct contradiction with reality, it is capable of everything. One character creation is complete, what the character is good at and bad at, refines the contradiction, begining to resolve it's tension. This should be a continuouse action. In Fallout 1 this is shown through the tagged skills, extra skill points on level up, and specialization Perks. Specialization perks ask the player to sacrifice a part of N, in exchange for dialectical resolution. The player can maintain the status quo, or they can evolve, and reckon with the ontology of their character. 

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
