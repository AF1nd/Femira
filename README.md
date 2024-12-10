# Femira

A simple programming language that compiles into vm bytecode

## Language basics:

":" - start block |
"end" - end block

"output" - print operator

define function:

```
fn sum(arg1, arg2):
   return arg1 + arg2
end

output sum(1, 2)
```

if statement:

```
if 1 == 1:
   ..code
end
else:
   ..code
end
```

objects:

```
person := {
    data := {
        age := 18,
        uid := 129319,
    },

    getAge := fn():
        return this.data.age
    end,

    getUID := fn():
        return this.data.uid
    end
}

output person["data"]['age']

output person.getAge()
output person.getUID()
```

arrays:

```
array := [1, 2, 3]

array[0] = 'str'

output array[0]
```

## Run program:

path/to/interpreter-file (femic.exe/femic.out) path/to/program.fmr:

folder/femic.out main.fmr
