const express = require('express');
const fs = require('fs');

const app = express();

app.get('/', (req, res) => {
  fs.readFile('./sensorValues.txt', 'utf8', (err, data) => {
    if (err) {
      res.sendStatus(500);
    } else {
        const lines = data.trim().split("\n");
        const lastLine = lines.slice(-1)[0];
        const fields = lastLine.split(",");
        res.json({lower:fields[0], upper: fields[1]});
    }
  });
});

app.listen(3000, () => {
  console.log('Server listening on port 3000');
});