# Inventory Management API (Вариант 18)

REST API для управления складом на базе Yandex Userver.

## Функциональность
- Регистрация и аутентификация (JWT-like токены)
- Поиск пользователя по логину или маске имени/фамилии
- Добавление товара на склад
- Поиск товара по названию
- Получение остатков
- Создание поступления (приход товара)
- История поступлений
- Списание товара

## Запуск

### Сборка
```bash
mkdir build && cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j$(nproc)
```

### Запуск сервера
```bash
./inventory-api --config ../config/config.yaml
```


### Docker
```bash
docker-compose up --build
Сервис будет доступен на http://localhost:8082.
```


## Тестирование
```bash
cd tests
./run_tests.sh
```


## Примеры запросов
См. openapi.yaml для полной спецификации. Примеры ниже:

### Регистрация
```bash
curl -X POST http://localhost:8080/api/auth/register \
  -H "Content-Type: application/json" \
  -d '{"login":"ivanov","password":"123456","first_name":"Иван","last_name":"Иванов","email":"ivan@example.com"}'
```

### Вход
```bash
curl -X POST http://localhost:8080/api/auth/login \
  -H "Content-Type: application/json" \
  -d '{"login":"ivanov","password":"123456"}'
```


Ответ:

json
{
  "access_token": "jwt_ivanov_1715001234",
  "token_type": "Bearer",
  "expires_in": 3600,
  "user": {"id":"user_...","login":"ivanov","role":"storekeeper"}
}

### Добавление товара (требуется токен)
```bash
TOKEN="полученный_токен"
curl -X POST http://localhost:8080/api/products \
  -H "Authorization: Bearer $TOKEN" \
  -H "Content-Type: application/json" \
  -d '{"name":"Гвозди 100мм","initial_quantity":1000,"sku":"GVD-100","unit":"кг"}'
```

### Списание товара
```bash
curl -X POST http://localhost:8080/api/inventory/write-off \
  -H "Authorization: Bearer $TOKEN" \
  -H "Content-Type: application/json" \
  -d '{"product_id":"prod_...", "quantity":50}'
```


### Примечание
Хранение данных in-memory. При перезапуске данные сбрасываются.
