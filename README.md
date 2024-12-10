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
obj := {
   foo := 'aa', // (or ;)
   func := fn():
      output 'f'
   end
}

obj.func()
output obj.foo
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
