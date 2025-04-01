FROM node:22.14.0-slim

WORKDIR /usr/src/app

COPY package*.json ./

RUN npm ci

COPY . .

RUN npm run build

EXPOSE 8080

EXPOSE 3000

CMD npm run start
