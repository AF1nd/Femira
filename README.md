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
        return self.data.age
    end,

    getUID := fn():
        return self.data.uid
    end
}

output person.getAge()

fn foo(p):
    p.data.age := 5
end 

foo(person)

output person.getAge()
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
