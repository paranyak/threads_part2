# threads_part2
Second part of lab with counting words
Завдання
Слід розробити програму, котра для обробки використовує конвеєр:
1. Потік вводу-виводу, який читає блоки та додає їх до черги блоків на обробку.
2. Потоки, які беруть дані із черги, та підраховують кількість слів у своєму блоці. Отриманий результат передається у чергу словників на об’єднання.
2а. Додаткове завдання: підраховуючи слова, не враховувати регістр літер та відкидати розділові знаки.
3. Потоки беруть пари елементів із черги на об’єднання, об’єднують їх між собою і повертають в чергу.
По завершенню результат виводиться у файли. 
Черги повинні бути потокобезпечними – захищеними зовнішніми чи внутрішніми мютексами,  або lock-free. Для координації роботи потоків слід скористатися умовними змінними.

