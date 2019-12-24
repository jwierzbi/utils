# Python Utils

## src/style.py

Contains simple class for applying styles to string. The styles are ANSI
escape sequences. The example usage is:

```python
print(Style.style(
    'Lorem ipsum ' +
    Style.style('dolor', underline=True) +
    ' sit amet, consectetur adipiscing elit.', bold=True
))
```

The above will render a bold string with underline `dolor` word.