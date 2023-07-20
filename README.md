# fastvn-programming-exercise
This is the rework branch, in which I will reorder and rewrite components in a better, more thought out architecture.


The basic idea of this rewrite is to organise the engine around a central data store, representing the novel being played.
While the exact structure and form of this store is yet undecided, it will have three distinct 'areas' of storage:
- local storage, for momentary data (e.g. speech and expressions)
- chapter storage, for reusable data (e.g. characters and story progress)
- global storage, for data that's shared across an entire game (e.g. ending statuses)

Furthermore, the engine will be modularised, such that even core parts of a VN, like the graphics subsystem, can be updated independently and treated uniformly with other, 'lesser' modules.



Data format:
The nucleus of this design phase is the new script format for writing novels in. By extensively testing and extending this format, it can be made both ergonomic and powerful.
By starting with making the part of the development process that the novel writer actually manipulates, the rest of the engine can be logically laid out around this.
One experimental aspect of this is structuring the script as *data*, rather than code. While they may seem similar, by treating statements as descriptions instead of
instructions to be followed, I hope that the resulting execution pathway will be easier to reason about. For instance, take this snippet:

```
    Lisa, happy: "This is the best day of my life!"
	scene_change(Garden_Sunshine_02)
```

Instead of viewing this as a `print_dialogue` call (with fancy syntax) that is followed by a command to change the scene, the idea is to model both of these as attributes of the same
story "moment". Each moment has its own place in the data store, and is the 'scope' of local storage. The dialogue statement informs the engine what to show on-screen in that moment,
and the scene change command links the moment to another. Certain constructs will create links implicitly, according to what is most ergonomic for the common case. In addition, other
constructs may set or change other attributes that don't show directly on the screen, but that other modules may use internally for other purposes. An example of this would be an
object-and-location event model to ease the development of interactive experiences. Such a model would require state, and such state may be included in the script. 