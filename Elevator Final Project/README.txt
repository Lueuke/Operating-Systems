# README elevator.cpp

# TASK
Our Final Project for my operating systems class we had to utilized all the previous assingments that we had created in this repo to 
create a functionion elevator system using API calls Multithreading and a Scheduling algorithm. We were givin the building file that 
gave us the number of elevators , capacity, highest and lowest floor using that information we had to schedule people based on the 
API call from NextInput and process all the people to insure everyone gets to there floor.

#SCHEDULING 
For this project when trying to decide what scheduling algorithm to use I wanted something that could take in multiple factors and 
being able to make the best descion is possible could. So I made a scheduling algorithm that based off the person start and end floor 
as well as which floors the elevators could reach and how much capcity it had I designed it to make the best possible guess. There 
is ways I could have optimized in certain areas because they way I have it set up was if that elevator was not on the persons floor 
was to just put that person on the closest elevator that was on there floor . The reason I did this and why I shouldn't have caught it 
sooner was becuase if I didn't the scheduler would try to schedule people on an elvator that was not on their floor so I don't think 
I had any other option to assign people. This does run into problems where if the elevators sometimes deviate or change based on a 
random factor I can't control then it will eat into my timesteps. 